#include <arpa/inet.h>
#include "Server.h"
#include <AuthorityInfo.h>
#include "AnanasUtils.h"

namespace ananas
{
    Server::Server(const uint8_t numChannelsToSend) : numChannels(numChannelsToSend),
                                                      fifo(numChannelsToSend),
                                                      sender(fifo),
                                                      switchInspector(switches),
                                                      clientListener(clients),
                                                      authorityListener(authority),
                                                      rebootSender(clients)
    {
    }

    Server::~Server()
    {
        releaseResources();
    }

    void Server::prepareToPlay(const int samplesPerBlockExpected, const double sampleRate)
    {
        while (!sender.prepare(numChannels, samplesPerBlockExpected, sampleRate) ||
               !timestampListener.prepare() ||
               !clientListener.prepare() ||
               !authorityListener.prepare() ||
               !rebootSender.prepare()) {
            sleep(1);
        }

        switchInspector.startThread();

        timestampListener.onTimestamp = [this](const timespec &ts)
        {
            sender.setPacketTime(ts);
        };
    }

    void Server::releaseResources()
    {
        if (sender.isThreadRunning()) {
            sender.stopThread(Constants::AudioSenderSocketTimeoutMs);
        }
        if (timestampListener.isThreadRunning()) {
            timestampListener.stopThread(Constants::TimestampListenerSocketTimeoutMs);
        }
        if (clientListener.isThreadRunning()) {
            clientListener.stopThread(Constants::ClientListenerSocketTimeoutMs);
        }
        if (authorityListener.isThreadRunning()) {
            authorityListener.stopThread(Constants::AuthorityListenerSocketTimeoutMs);
        }
        if (switchInspector.isThreadRunning()) {
            switchInspector.stopThread(Constants::SwitchInspectorRequestTimeoutMs);
        }
        if (rebootSender.isThreadRunning()) {
            rebootSender.stopThread(Constants::RebootSenderSocketTimeoutMs);
        }
    }

    void Server::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
    {
        fifo.write(bufferToFill.buffer);
    }

    ClientList *Server::getClientList()
    {
        return &clients;
    }

    AuthorityInfo *Server::getAuthority()
    {
        return &authority;
    }

    SwitchList *Server::getSwitches()
    {
        return &switches;
    }

    //==============================================================================

    Server::AudioSender::AudioSender(Fifo &fifo) : Thread(Constants::AudioSenderThreadName),
                                                   fifo(fifo)
    {
    }

    Server::AudioSender::~AudioSender()
    {
        socket.leaveMulticast(Constants::AudioMulticastIP);
        socket.shutdown();
    }

    bool Server::AudioSender::prepare(const int numChannels, const int samplesPerBlockExpected, const double sampleRate)
    {
        juce::ignoreUnused(sampleRate);

        if (isReady) return true;

        audioBlockSamples = samplesPerBlockExpected;

        if (-1 == socket.getBoundPort()) {
            if (!socket.setEnablePortReuse(true)) {
                std::cerr << getThreadName() << " Failed to set socket port reuse: " << strerror(errno) << std::endl;
                socket.shutdown();
                return false;
            }

            if (!socket.bindToPort(Constants::AudioSenderSocketLocalPort, Constants::LocalInterfaceIP)) {
                std::cerr << getThreadName() << " Failed to bind socket to port: " << strerror(errno) << std::endl;
                socket.shutdown();
                return false;
            }

            if (!socket.joinMulticast(Constants::AudioMulticastIP)) {
                std::cerr << getThreadName() << " Failed to join multicast group: " << strerror(errno) << std::endl;
                socket.shutdown();
                return false;
            }

            socket.setMulticastLoopbackEnabled(false);
            socket.waitUntilReady(false, 1000);
        }

        packet.prepare(numChannels, Constants::FramesPerPacket, sampleRate);

        isReady = startThread();
        return isReady;
    }

    void Server::AudioSender::run()
    {
        DBG("Sending audio packets...");

        while (!threadShouldExit()) {
            // Read from the fifo into the packet.
            fifo.read(packet.getAudioData(), Constants::FramesPerPacket);
            if (threadShouldExit()) break;
            // Write the header to the packet.
            packet.writeHeader();
            // Write the packet to the socket.
            socket.write(
                Constants::AudioMulticastIP,
                Constants::AudioSenderSocketRemotePort,
                packet.getData(),
                static_cast<int>(packet.getSize())
            );

            const timespec t{0, packet.getSleepInterval()};
            nanosleep(&t, nullptr);
        }

        DBG("Stopping send thread");
    }

    void Server::AudioSender::setPacketTime(const timespec ts)
    {
        packet.setTime(ts);
    }

    int64_t Server::AudioSender::getPacketTime() const
    {
        return packet.getTime();
    }

    bool Server::AudioSender::stopThread(const int timeOutMilliseconds)
    {
        fifo.abortRead();
        return Thread::stopThread(timeOutMilliseconds);
    }

    //==============================================================================

    Server::AnnouncementListenerThread::AnnouncementListenerThread(
        const juce::String &threadName,
        const juce::String &multicastIP,
        const int portToListenOn
    ) : Thread(threadName), ip(multicastIP), port(portToListenOn)
    {
    }

    Server::AnnouncementListenerThread::~AnnouncementListenerThread()
    {
        socket.leaveMulticast(ip);
        socket.shutdown();
    }

    bool Server::AnnouncementListenerThread::prepare()
    {
        if (isReady) return true;

        if (-1 == socket.getBoundPort()) {
            // JUCE doesn't handle multicast in a manner that's compatible with
            // reading multicast packets on a specific interface...

            // ...(it's probably not necessary to allow port-reuse, but what the
            // hell)...
            if (!socket.setEnablePortReuse(true)) {
                std::cerr << getThreadName() << " Failed to set socket port reuse: " << strerror(errno) << std::endl;
                socket.shutdown();
                return false;
            }

            // ...bind the relevant port to ALL interfaces (INADDR_ANY) by not
            // specifying a local interface here...
            if (!socket.bindToPort(port)) {
                std::cerr << getThreadName() << " Failed to bind socket to port: " << strerror(errno) << std::endl;
                socket.shutdown();
                return false;
            }

            // ...then join the appropriate multicast group on the relevant
            // interface (i.e. a manually-configured ethernet connection).
            ip_mreq mreq{};
            mreq.imr_multiaddr.s_addr = inet_addr(ip.toRawUTF8());
            mreq.imr_interface.s_addr = inet_addr(Constants::LocalInterfaceIP.text);

            if (setsockopt(
                    socket.getRawSocketHandle(),
                    IPPROTO_IP,
                    IP_ADD_MEMBERSHIP,
                    &mreq, sizeof (mreq)) < 0) {
                std::cerr << getThreadName() << " Failed to add multicast membership: " << strerror(errno) << std::endl;
                socket.shutdown();
                return false;
            }

            socket.setMulticastLoopbackEnabled(false);
        }

        isReady = startThread();
        return isReady;
    }

    //==============================================================================

    Server::TimestampListener::TimestampListener()
        : AnnouncementListenerThread(Constants::TimestampListenerThreadName,
                                     Constants::PTPMulticastIP,
                                     Constants::TimestampListenerLocalPort)
    {
    }

    void Server::TimestampListener::run()
    {
        DBG("Listening for PTP timestamps...");

        while (!threadShouldExit()) {
            if (socket.waitUntilReady(true, Constants::TimestampListenerSocketTimeoutMs)) {
                if (threadShouldExit()) break;

                uint8_t buffer[Constants::TimestampListenerBufferSize];
                juce::String senderIP;
                int senderPort;

                if (const auto bytesRead{
                        socket.read(buffer, Constants::TimestampListenerBufferSize, false, senderIP, senderPort)
                    };
                    bytesRead > 0
                ) {
                    // DBG("Received " << bytesRead << " bytes from " << senderIP << ":" << senderPort);

                    // Check for Follow_Up message (0x08)
                    if ((buffer[0] & 0x0f) == Constants::FollowUpMessageType) {
                        // DBG("PTP follow-up message received from " << senderIP << ":" << senderPort);

                        timespec ts{};

                        // Extract seconds (6 bytes)
                        for (int i = 0; i < 6; i++) {
                            ts.tv_sec = ts.tv_sec << 8 | buffer[34 + i];
                        }

                        // Extract nanoseconds (4 bytes)
                        for (int i = 0; i < 4; i++) {
                            ts.tv_nsec = ts.tv_nsec << 8 | buffer[40 + i];
                        }

                        // DBG("Timestamp: " << ts.tv_sec << "." << ts.tv_nsec << " --- " << ctime(&ts.tv_sec));

                        if (onTimestamp != nullptr) {
                            onTimestamp(ts);
                        }
                    }
                } else if (bytesRead < 0) {
                    DBG("Error receiving data: " << strerror(errno));
                }
            }
        }

        DBG("Stopping timestamp listener thread");
    }

    //==============================================================================

    Server::ClientListener::ClientListener(ClientList &clients)
        : AnnouncementListenerThread(Constants::ClientListenerThreadName,
                                     Constants::ClientAnnounceMulticastIP,
                                     Constants::ClientListenerLocalPort),
          clients(clients)
    {
    }

    void Server::ClientListener::run()
    {
        DBG("Listening for clients...");

        while (!threadShouldExit()) {
            if (socket.waitUntilReady(true, Constants::ClientListenerSocketTimeoutMs)) {
                if (threadShouldExit()) break;

                uint8_t buffer[Constants::ClientListenerBufferSize];
                juce::String senderIP;
                int senderPort;

                if (const auto bytesRead{
                        socket.read(buffer, Constants::ClientListenerBufferSize, false, senderIP, senderPort)
                    };
                    bytesRead > 0
                ) {
                    clients.handlePacket(senderIP, reinterpret_cast<const ClientAnnouncePacket *>(buffer));
                } else if (bytesRead < 0) {
                    DBG("Error receiving data: " << strerror(errno));
                }
            }
        }

        DBG("Stopping client listener thread");
    }

    //==============================================================================

    Server::AuthorityListener::AuthorityListener(AuthorityInfo &authority)
        : AnnouncementListenerThread(Constants::AuthorityListenerThreadName,
                                     Constants::AuthorityAnnounceMulticastIP,
                                     Constants::AuthorityListenerLocalPort),
          authority(authority)
    {
    }

    void Server::AuthorityListener::run()
    {
        DBG("Listening for timing authority...");

        while (!threadShouldExit()) {
            if (socket.waitUntilReady(true, Constants::AuthorityListenerSocketTimeoutMs)) {
                if (threadShouldExit()) break;

                int senderPort;

                if (const auto bytesRead{
                        socket.read(&authority.getInfo(), sizeof(AuthorityAnnouncePacket), false, authority.getIP(), senderPort)
                    };
                    bytesRead == -1
                ) {
                    DBG("Error receiving data: " << strerror(errno));
                } else {
                    authority.update();
                }
            }
        }

        DBG("Stopping timing authority listener thread");
    }

    //==========================================================================

    Server::RebootSender::RebootSender(ClientList &clients)
        : Thread(Constants::RebootSenderThreadName),
          clients(clients)
    {
    }

    bool Server::RebootSender::prepare()
    {
        if (isReady) return true;

        if (-1 == socket.getBoundPort()) {
            if (!socket.setEnablePortReuse(true)) {
                std::cerr << getThreadName() << " Failed to set socket port reuse: " << strerror(errno) << std::endl;
                socket.shutdown();
                return false;
            }

            if (!socket.bindToPort(Constants::RebootSenderSocketLocalPort, Constants::LocalInterfaceIP)) {
                std::cerr << getThreadName() << " Failed to bind socket to port: " << strerror(errno) << std::endl;
                socket.shutdown();
                return false;
            }

            if (!socket.joinMulticast(Constants::RebootMulticastIP)) {
                std::cerr << getThreadName() << " Failed to join multicast group: " << strerror(errno) << std::endl;
                socket.shutdown();
                return false;
            }

            socket.setMulticastLoopbackEnabled(false);
            socket.waitUntilReady(false, 1000);
        }

        isReady = startThread();
        return isReady;
    }

    void Server::RebootSender::run()
    {
        while (!threadShouldExit()) {
            if (clients.getShouldReboot()) {
                clients.setShouldReboot(false);
                socket.write(
                    Constants::RebootMulticastIP,
                    Constants::RebootSenderSocketRemotePort,
                    nullptr,
                    0
                );
            }

            // Wait 1 second, but check for thread exit every 100ms
            for (int i = 0; i < 10 && !threadShouldExit(); ++i)
                wait(100);
        }
    }

    //==========================================================================

    Server::SwitchInspector::SwitchInspector(SwitchList &switches)
        : Thread(Constants::SwitchInspectorThreadName), switches(switches)
    {
    }

    void Server::SwitchInspector::run()
    {
        DBG("Inspecting switches...");

        // curl -k -u admin:emeraude http://192.168.10.1/rest/system/ptp/monitor -d \'{"numbers":"0","once":""}\' -H "content-type: application/json"

        while (!threadShouldExit()) {
            auto switchesVar{switches.toVar()};

            if (auto *obj = switchesVar.getDynamicObject()) {
                for (const auto &prop: obj->getProperties()) {
                    if (const auto *s = prop.value.getDynamicObject()) {
                        auto index{prop.name.toString().fromLastOccurrenceOf("_", false, false).getIntValue()};

                        auto ip{s->getProperty(Identifiers::SwitchIpPropertyID)};
                        auto username{s->getProperty(Identifiers::SwitchUsernamePropertyID).toString()};
                        auto password{s->getProperty(Identifiers::SwitchPasswordPropertyID).toString()};
                        bool shouldResetPtp{s->getProperty(Identifiers::SwitchShouldResetPtpPropertyID)};

                        if (!ip.isString() || ip.toString().isEmpty() || username.isEmpty() || password.isEmpty()) break;

                        juce::String postData, path1, path2;
                        if (shouldResetPtp) {
                            const juce::var jsonData{new juce::DynamicObject};
                            jsonData.getDynamicObject()->setProperty("numbers", "0");
                            postData = juce::JSON::toString(jsonData);
                            path1 = Constants::SwitchDisablePtpPath;
                            path2 = Constants::SwitchEnablePtpPath;
                        } else {
                            const juce::var jsonData{new juce::DynamicObject};
                            jsonData.getDynamicObject()->setProperty("numbers", "0");
                            jsonData.getDynamicObject()->setProperty("once", "");
                            postData = juce::JSON::toString(jsonData);
                            path1 = Constants::SwitchMonitorPtpPath;
                        }

                        juce::URL url("http://" + ip.toString() + path1);

                        juce::ChildProcess curl;
                        juce::StringArray args;
                        args.add("curl");
                        args.add("-s"); // Silent, no stats
                        args.add("-k"); // Insecure (no TLS)
                        args.add("-u"); // Specify username and password
                        args.add(username + ":" + password);
                        args.add(url.toString(false));
                        args.add("-H");
                        args.add("Content-Type: application/json");
                        args.add("-d");
                        args.add(postData);

                        if (curl.start(args)) {
                            const auto response{curl.readAllProcessOutput()};

                            switches.handleResponse(index, juce::JSON::parse(response));
                        }

                        // TODO don't make this terrible repetition
                        if (shouldResetPtp) {
                            args.clear();
                            args.add("curl");
                            args.add("-s"); // Silent, no stats
                            args.add("-k"); // Insecure (no TLS)
                            args.add("-u"); // Specify username and password
                            args.add(username + ":" + password);
                            args.add(juce::URL{"http://" + ip.toString() + path2}.toString(false));
                            args.add("-H");
                            args.add("Content-Type: application/json");
                            args.add("-d");
                            args.add(postData);

                            if (curl.start(args)) {
                                const auto response{curl.readAllProcessOutput()};

                                switches.handleResponse(index, juce::JSON::parse(response));
                            }
                        }
                    }
                }
            }

            // Wait 1 second, but check for thread exit every 100ms
            for (int i = 0; i < 10 && !threadShouldExit(); ++i)
                wait(100);
        }

        DBG("Stopping switch inspector thread");
    }
}

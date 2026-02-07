#include <arpa/inet.h>
#include "Server.h"
#include <AnanasUtils.h>
#include <AuthorityInfo.h>
#include "ServerUtils.h"

namespace ananas::Server
{
    Server::Server(const uint numChannelsToSend) : numChannels(numChannelsToSend),
                                                   fifo(numChannelsToSend)
    {
        threads.add(new AudioSender(Sockets::AudioSenderSocketParams, fifo));
        threads.add(new TimestampListener(Sockets::TimestampListenerSocketParams));
        threads.add(new ClientListener(Sockets::ClientListenerSocketParams, clients, modules));
        threads.add(new AuthorityListener(Sockets::AuthorityListenerSocketParams, authority));
        threads.add(new RebootSender(Sockets::RebootSenderSocketParams, clients));
        threads.add(new SwitchInspector(Threads::SwitchInspectorThreadParams, switches));

        for (const auto &t: threads) {
            t->addChangeListener(this);
            t->startThread();
        }
    }

    Server::~Server()
    {
        releaseResources();
    }

    void Server::prepareToPlay(const int samplesPerBlockExpected, const double sampleRate)
    {
        if (auto *s = dynamic_cast<AudioSender *>(threads[0])) {
            s->prepare(numChannels, samplesPerBlockExpected, sampleRate);
        }
    }

    void Server::releaseResources()
    {
        for (const auto &t: threads) {
            if (t->isThreadRunning()) {
                t->stopThread(t->getTimeout());
            }
        }
    }

    void Server::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
    {
        fifo.write(bufferToFill.buffer);
    }

    void Server::changeListenerCallback(ChangeBroadcaster *source)
    {
        if (const auto *t = dynamic_cast<TimestampListener *>(source)) {
            if (t->getTimestampChanged()) {
                return;
            }
        }
        sendChangeMessage();
    }

    bool Server::isConnected() const
    {
        for (const auto &t: threads) {
            if (!t->isConnected()) {
                return false;
            }
        }
        return true;
    }

    ClientList *Server::getClientList()
    {
        return &clients;
    }

    ModuleList *Server::getModuleList()
    {
        return &modules;
    }

    AuthorityInfo *Server::getAuthority()
    {
        return &authority;
    }

    SwitchList *Server::getSwitches()
    {
        return &switches;
    }


    //==========================================================================

    Server::AnanasThread::AnanasThread(const Utils::ThreadParams &p)
        : Thread(p.name), timeoutMs(p.timeoutMs)
    {
    }

    void Server::AnanasThread::run()
    {
        while (!connect() && !threadShouldExit()) {
            for (uint i{0}; i < Constants::ThreadConnectWaitIterations && !threadShouldExit(); ++i)
                wait(Constants::ThreadConnectWaitIntervalMs);
        }

        if (!threadShouldExit()) {
            connected = true;
            runImpl();
        }
    }

    int Server::AnanasThread::getTimeout() const
    {
        return timeoutMs;
    }

    bool Server::AnanasThread::isConnected() const
    {
        return connected;
    }

    //==========================================================================

    Server::UDPMulticastThread::UDPMulticastThread(const Utils::ThreadSocketParams &p)
        : AnanasThread(p),
          ip(p.ip),
          localPort(p.localPort)
    {
    }

    Server::UDPMulticastThread::~UDPMulticastThread()
    {
        socket.leaveMulticast(ip);
        socket.shutdown();
    }

    bool Server::UDPMulticastThread::connect()
    {
        if (-1 == socket.getBoundPort()) {
            if (!socket.setEnablePortReuse(true)) {
                std::cerr << getThreadName() << " failed to set socket port reuse: " << strerror(errno) << std::endl;
                socket.shutdown();
                sendChangeMessage();
                return false;
            }

            if (!socket.bindToPort(localPort, Utils::Strings::LocalInterfaceIP)) {
                std::cerr << getThreadName() << " failed to bind socket to port: " << strerror(errno) << std::endl;
                socket.shutdown();
                sendChangeMessage();
                return false;
            }

            if (!socket.joinMulticast(ip)) {
                std::cerr << getThreadName() << " failed to join multicast group: " << strerror(errno) << std::endl;
                sendChangeMessage();
                socket.shutdown();
                return false;
            }

            socket.setMulticastLoopbackEnabled(false);
            socket.waitUntilReady(false, 1000);
        }

        sendChangeMessage();
        return true;
    }

    //==========================================================================

    Server::SenderThread::SenderThread(const Utils::SenderThreadSocketParams &p)
        : UDPMulticastThread(p),
          remotePort(p.remotePort)
    {
    }

    //==========================================================================

    Server::AudioSender::AudioSender(const Utils::SenderThreadSocketParams &p, Fifo &fifo)
        : SenderThread(p),
          fifo(fifo)
    {
    }

    bool Server::AudioSender::prepare(const int numChannels, const int samplesPerBlockExpected, const double sampleRate)
    {
        juce::ignoreUnused(sampleRate);

        audioBlockSamples = samplesPerBlockExpected;

        packet.prepare(numChannels, Constants::FramesPerPacket, sampleRate);

        return startThread();
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

    void Server::AudioSender::changeListenerCallback(juce::ChangeBroadcaster *source)
    {
        if (const auto *t = dynamic_cast<TimestampListener *>(source)) {
            setPacketTime(t->getPacketTime());
        }
    }

    void Server::AudioSender::runImpl()
    {
        DBG("Sending audio packets...");

        while (!threadShouldExit()) {
            // Read from the fifo into the packet.
            fifo.read(packet.getAudioData(), Constants::FramesPerPacket);
            if (threadShouldExit()) break;
            // Write the header to the packet.
            packet.writeHeader();
            // Write the packet to the socket.
            socket.write(ip, remotePort, packet.getData(), static_cast<int>(packet.getSize()));

            const timespec t{0, packet.getSleepInterval()};
            nanosleep(&t, nullptr);
        }

        DBG("Stopping send thread");
    }

    //==========================================================================

    Server::AnnouncementListenerThread::AnnouncementListenerThread(
        const Utils::ListenerThreadSocketParams &p
    ) : UDPMulticastThread(p)
    {
    }

    bool Server::AnnouncementListenerThread::connect()
    {
        if (-1 == socket.getBoundPort()) {
            // JUCE doesn't handle multicast in a manner that's compatible with
            // reading multicast packets on a specific interface...

            // ...(it's probably not necessary to allow port-reuse, but what the
            // hell)...
            if (!socket.setEnablePortReuse(true)) {
                std::cerr << getThreadName() << " failed to set socket port reuse: " << strerror(errno) << std::endl;
                socket.shutdown();
                sendChangeMessage();
                return false;
            }

            // ...bind the relevant port to ALL interfaces (INADDR_ANY) by not
            // specifying a local interface here...
            if (!socket.bindToPort(localPort)) {
                std::cerr << getThreadName() << " failed to bind socket to port: " << strerror(errno) << std::endl;
                socket.shutdown();
                sendChangeMessage();
                return false;
            }

            // ...then join the appropriate multicast group on the relevant
            // interface (i.e. a manually-configured ethernet connection).
            ip_mreq mreq{};
            mreq.imr_multiaddr.s_addr = inet_addr(ip.toRawUTF8());
            mreq.imr_interface.s_addr = inet_addr(Utils::Strings::LocalInterfaceIP.text);

            if (setsockopt(
                    socket.getRawSocketHandle(),
                    IPPROTO_IP,
                    IP_ADD_MEMBERSHIP,
                    &mreq, sizeof (mreq)) < 0) {
                std::cerr << getThreadName() << " failed to add multicast membership: " << strerror(errno) << std::endl;
                socket.shutdown();
                sendChangeMessage();
                return false;
            }

            socket.setMulticastLoopbackEnabled(false);
        }

        sendChangeMessage();
        return true;
    }

    void Server::AnnouncementListenerThread::runImpl()
    {
        std::cout << getThreadName() << " listening..." << std::endl;

        while (!threadShouldExit()) {
            if (socket.waitUntilReady(true, timeoutMs)) {
                if (threadShouldExit()) break;

                if (const auto bytesRead{
                    socket.read(buffer, Constants::ListenerBufferSize, false, senderIP, senderPort)
                }; bytesRead > 0) {
                    handlePacket();
                } else if (bytesRead < 0) {
                    std::cerr << getThreadName() << ": error reading from socket: " << strerror(errno) << std::endl;
                }
            }
        }

        std::cout << getThreadName() << "stopping." << std::endl;
    }

    //==============================================================================

    Server::TimestampListener::TimestampListener(
        const Utils::ListenerThreadSocketParams &p
    ) : AnnouncementListenerThread(p)
    {
    }

    timespec Server::TimestampListener::getPacketTime() const
    {
        return timestamp;
    }

    bool Server::TimestampListener::getTimestampChanged() const
    {
        return timestampChanged;
    }

    void Server::TimestampListener::handlePacket()
    {
        // DBG("Received " << bytesRead << " bytes from " << senderIP << ":" << senderPort);

        // Check for Follow_Up message (0x08)
        if ((buffer[0] & 0x0f) == Constants::PTPFollowUpMessageType) {
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

            timestamp = ts;
            timestampChanged = true;
            sendSynchronousChangeMessage();
            timestampChanged = false;
        }
    }

    //==============================================================================

    Server::ClientListener::ClientListener(
        const Utils::ListenerThreadSocketParams &p,
        ClientList &clients,
        ModuleList &modules
    ) : AnnouncementListenerThread(p),
        clients(clients),
        modules(modules)
    {
    }

    void Server::ClientListener::handlePacket()
    {
        clients.handlePacket(senderIP, reinterpret_cast<const ClientAnnouncePacket *>(buffer));
        modules.handlePacket(senderIP);
    }

    //==============================================================================

    Server::AuthorityListener::AuthorityListener(
        const Utils::ListenerThreadSocketParams &p,
        AuthorityInfo &authority
    ) : AnnouncementListenerThread(p),
        authority(authority)
    {
    }

    void Server::AuthorityListener::handlePacket()
    {
        authority.handlePacket(senderIP, reinterpret_cast<AuthorityAnnouncePacket *>(buffer));
    }

    //==========================================================================

    Server::RebootSender::RebootSender(
        const Utils::SenderThreadSocketParams &p,
        ClientList &clients
    ): SenderThread(p),
       clients(clients)
    {
    }

    void Server::RebootSender::runImpl()
    {
        while (!threadShouldExit()) {
            if (clients.getShouldReboot()) {
                clients.setShouldReboot(false);
                socket.write(ip, remotePort, nullptr, 0);
            }

            // Wait 1 second, but check for thread exit every 100ms
            for (int i = 0; i < 10 && !threadShouldExit(); ++i)
                wait(100);
        }
    }

    //==========================================================================

    Server::SwitchInspector::SwitchInspector(
        const Utils::ThreadParams &p,
        SwitchList &switches
    ) : AnanasThread(p),
        switches(switches)
    {
    }

    void Server::SwitchInspector::runImpl()
    {
        DBG("Inspecting switches...");

        while (!threadShouldExit()) {
            auto switchesVar{switches.toVar()};

            if (auto *obj = switchesVar.getDynamicObject()) {
                for (const auto &prop: obj->getProperties()) {
                    if (const auto *s = prop.value.getDynamicObject()) {
                        auto ip{s->getProperty(Utils::Identifiers::SwitchIpPropertyID).toString()};
                        auto username{s->getProperty(Utils::Identifiers::SwitchUsernamePropertyID).toString()};
                        auto password{s->getProperty(Utils::Identifiers::SwitchPasswordPropertyID).toString()};
                        const bool shouldResetPtp{s->getProperty(Utils::Identifiers::SwitchShouldResetPtpPropertyID)};

                        if (ip.isEmpty() || username.isEmpty() || password.isEmpty()) break;

                        juce::var jsonData{new juce::DynamicObject};
                        juce::var response;

                        if (shouldResetPtp) {
                            jsonData.getDynamicObject()->setProperty("numbers", "0");
                            response = curlRequest(ip, username, password, Constants::SwitchDisablePtpPath, juce::JSON::toString(jsonData));
                            switches.handleResponse(prop.name, response);
                            response = curlRequest(ip, username, password, Constants::SwitchEnablePtpPath, juce::JSON::toString(jsonData));
                            switches.handleResponse(prop.name, response);
                        } else {
                            jsonData.getDynamicObject()->setProperty("numbers", "0");
                            jsonData.getDynamicObject()->setProperty("once", "");
                            response = curlRequest(ip, username, password, Constants::SwitchMonitorPtpPath, juce::JSON::toString(jsonData));
                            switches.handleResponse(prop.name, response);
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

    bool Server::SwitchInspector::connect()
    {
        sendChangeMessage();
        return true;
    }

    juce::var Server::SwitchInspector::curlRequest(const juce::String &ip,
                                                   const juce::String &username,
                                                   const juce::String &password,
                                                   const juce::StringRef &path,
                                                   const juce::String &postData)
    {
        const juce::URL url("http://" + ip + path);

        juce::StringArray args;
        args.add("curl");
        args.add("-s"); // Silent, no stats
        args.add("-m" + juce::String{Constants::SwitchInspectorRequestTimeoutS});
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
            return juce::JSON::parse(response);
        }
        return juce::var{};
    }
}

#include "Server.h"
#include <Utils.h>
#include <arpa/inet.h>

ananas::Server::Server(const uint8_t numChannelsToSend) : numChannels(numChannelsToSend),
                                                          fifo(numChannelsToSend),
                                                          sender(fifo),
                                                          clientListener(clients)
{
}

ananas::Server::~Server()
{
    releaseResources();
}

void ananas::Server::prepareToPlay(const int samplesPerBlockExpected, const double sampleRate)
{
    while (!sender.prepare(numChannels, samplesPerBlockExpected, sampleRate) ||
           !timestampListener.prepare() ||
           !clientListener.prepare()) {
        sleep(1);
    }

    timestampListener.onTimestamp = [this](const timespec &ts)
    {
        // DBG("Timestamp diff: " << ts.tv_sec * 1'000'000'000 + ts.tv_nsec - (sender.getPacketTime() - 100'000'000) << " ns");
        sender.setPacketTime(ts);
    };
}

void ananas::Server::releaseResources()
{
    if (sender.isThreadRunning()) {
        sender.stopThread(Constants::SenderSocketTimeoutMs);
    }
    if (timestampListener.isThreadRunning()) {
        timestampListener.stopThread(Constants::TimestampListenerSocketTimeoutMs);
    }
    if (clientListener.isThreadRunning()) {
        clientListener.stopThread(Constants::ClientListenerSocketTimeoutMs);
    }
}

void ananas::Server::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    fifo.write(bufferToFill.buffer);
}

ananas::ClientList *ananas::Server::getClientList()
{
    return &clients;
}

//==============================================================================

ananas::Server::Sender::Sender(Fifo &fifo) : Thread(Constants::SenderThreadName),
                                             fifo(fifo)
{
}

bool ananas::Server::Sender::prepare(const int numChannels, const int samplesPerBlockExpected, const double sampleRate)
{
    if (isReady) return true;

    juce::ignoreUnused(sampleRate);
    audioBlockSamples = samplesPerBlockExpected;

    if (-1 == socket.getBoundPort()) {
        const auto bound{socket.bindToPort(Constants::SenderSocketLocalPort, Constants::LocalInterfaceIP)};
        if (const auto joined{bound && socket.joinMulticast(Constants::SenderSocketMulticastIP)}; !bound || !joined) {
            DBG(strerror(errno));
            return false;
        }
        socket.setMulticastLoopbackEnabled(false);
        socket.waitUntilReady(false, 1000);
    }

    packet.prepare(numChannels, Constants::FramesPerPacket, sampleRate);

    isReady = startThread();
    return isReady;
}

void ananas::Server::Sender::run()
{
    while (!threadShouldExit()) {
        // Read from the fifo into the packet.
        fifo.read(packet.getAudioData(), Constants::FramesPerPacket);
        if (threadShouldExit()) break;
        // Write the header to the packet.
        packet.writeHeader();
        // Write the packet to the socket.
        socket.write(
            Constants::SenderSocketMulticastIP,
            Constants::SenderSocketRemotePort,
            packet.getData(),
            static_cast<int>(packet.getSize())
        );

        const timespec t{0, packet.getSleepInterval()};
        nanosleep(&t, nullptr);
    }

    DBG("Stopping send thread");
}

void ananas::Server::Sender::setPacketTime(const timespec ts)
{
    packet.setTime(ts);
}

int64_t ananas::Server::Sender::getPacketTime() const
{
    return packet.getTime();
}

bool ananas::Server::Sender::stopThread(const int timeOutMilliseconds)
{
    fifo.abortRead();
    return Thread::stopThread(timeOutMilliseconds);
}

//==============================================================================

ananas::Server::TimestampListener::TimestampListener()
    : Thread(Constants::TimestampListenerThreadName)
{
}

bool ananas::Server::TimestampListener::prepare()
{
    if (isReady) return true;

    if (-1 == socket.getBoundPort()) {
        // JUCE doesn't handle multicast in a manner that's compatible with
        // reading PTP packets on a specific interface...

        // ...(it's probably not necessary to allow port-reuse in this case, but
        // what the hell)...
        if (!socket.setEnablePortReuse(true)) {
            DBG("Failed to set socket port reuse: " << strerror(errno));
            socket.shutdown();
            return false;
        }

        // ... anyway, so bind the relevant port to ALL interfaces
        // (INADDR_ANY)...
        if (!socket.bindToPort(Constants::TimestampListenerLocalPort)) {
            DBG("Failed to bind socket to port: " << strerror(errno));
            socket.shutdown();
            return false;
        }

        // ...then join the appropriate multicast group on the relevant
        // interface (i.e. a manually-configured ethernet connection).
        ip_mreq mreq{};
        mreq.imr_multiaddr.s_addr = inet_addr(Constants::PTPMulticastIP.text);
        mreq.imr_interface.s_addr = inet_addr(Constants::LocalInterfaceIP.text);

        if (setsockopt(
                socket.getRawSocketHandle(),
                IPPROTO_IP,
                IP_ADD_MEMBERSHIP,
                &mreq, sizeof (mreq)) < 0) {
            DBG("Failed to add multicast membership: " << strerror(errno));
            socket.shutdown();
            return false;
        }
    }

    isReady = startThread();
    return isReady;
}

void ananas::Server::TimestampListener::run()
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

ananas::Server::ClientListener::ClientListener(ClientList &clients)
    : Thread(Constants::ClientListenerThreadName),
      clients(clients)
{
}

bool ananas::Server::ClientListener::prepare()
{
    if (isReady) return true;

    if (-1 == socket.getBoundPort()) {
        if (!socket.setEnablePortReuse(true)) {
            DBG("Failed to set socket port reuse: " << strerror(errno));
            socket.shutdown();
            return false;
        }

        if (!socket.bindToPort(Constants::ClientListenerLocalPort)) {
            DBG("Failed to bind socket to port: " << strerror(errno));
            socket.shutdown();
            return false;
        }

        if (!socket.joinMulticast(Constants::PTPMulticastIP)) {
            DBG("Failed to join multicast group: " << strerror(errno));
            socket.shutdown();
            return false;
        }

        socket.setMulticastLoopbackEnabled(false);
    }

    isReady = startThread();
    return isReady;
}

void ananas::Server::ClientListener::run()
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
                clients.handlePacket(senderIP, reinterpret_cast<const AnnouncementPacket *>(buffer));
            } else if (bytesRead < 0) {
                DBG("Error receiving data: " << strerror(errno));
            }
        }
    }

    DBG("Stopping client listener thread");
}

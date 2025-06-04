#include "AnanasServer.h"

#include <arpa/inet.h>

AnanasServer::AnanasServer() : sender(fifo)
{
}

void AnanasServer::prepareToPlay(const int samplesPerBlockExpected, const double sampleRate)
{
    sender.prepare(samplesPerBlockExpected, sampleRate);
    timestampListener.prepare();
    timestampListener.onTimestamp = [this](const timespec &ts)
    {
        sender.setPacketTime(ts);
    };
}

void AnanasServer::releaseResources()
{
    if (sender.isThreadRunning()) {
        sender.stopThread(1000);
    }
    if (timestampListener.isThreadRunning()) {
        timestampListener.stopThread(1000);
    }
}

void AnanasServer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    fifo.write(bufferToFill.buffer);
}

//==============================================================================

AnanasServer::Sender::Sender(AnanasFifo &fifo) : Thread("Ananas Sender"),
                                                 fifo(fifo)
{
}

void AnanasServer::Sender::prepare(const int samplesPerBlockExpected, const double sampleRate)
{
    juce::ignoreUnused(sampleRate);
    audioBlockSamples = samplesPerBlockExpected;

    if (-1 == socket.getBoundPort()) {
        const auto bound{socket.bindToPort(14841, "192.168.10.10")};
        if (const auto joined{bound && socket.joinMulticast("224.4.224.4")}; !bound || !joined) {
            DBG(strerror(errno));
        }
        socket.setMulticastLoopbackEnabled(false);
        socket.waitUntilReady(false, 1000);
    }

    packet.prepare(samplesPerBlockExpected, sampleRate);

    startThread();
}

void AnanasServer::Sender::run()
{
    while (!threadShouldExit()) {
        // Read from the fifo into the packet.
        fifo.read(packet.getAudioData(), audioBlockSamples);
        // Write the header to the packet.
        packet.writeHeader();
        // Write the packet to the socket.
        socket.write("224.4.224.4", 49152, packet.getData(), static_cast<int>(packet.getSize()));
    }

    DBG("Stopping send thread");
}

void AnanasServer::Sender::setPacketTime(timespec ts)
{
    packet.setTime(ts);
}

//==============================================================================

AnanasServer::TimestampListener::TimestampListener()
    : Thread("Ananas Timestamp Listener")
{
}

void AnanasServer::TimestampListener::prepare()
{
    if (-1 == socket.getBoundPort()) {
        // JUCE doesn't handle multicast in a manner that's compatible with
        // reading PTP packets on a specific interface...

        // ...(it's probably not necessary to allow port-reuse in this case, but
        // what the hell)...
        if (!socket.setEnablePortReuse(true)) {
            DBG("Failed to set socket port reuse: " << strerror(errno));
            socket.shutdown(); // TODO: abstract away the socket...
            return;
        }

        // ... anyway, so bind the relevant port to ALL interfaces
        // (INADDR_ANY)...
        if (!socket.bindToPort(320)) {
            DBG("Failed to bind socket to port: " << strerror(errno));
            socket.shutdown();
            return;
        }

        // ...then join the appropriate multicast group on the relevant
        // interface (i.e. a manually-configured ethernet connection).
        struct ip_mreq mreq{};
        mreq.imr_multiaddr.s_addr = inet_addr(juce::String("224.0.1.129").toRawUTF8());
        mreq.imr_interface.s_addr = inet_addr(juce::String("192.168.10.10").toRawUTF8());

        if (setsockopt(
                socket.getRawSocketHandle(),
                IPPROTO_IP,
                IP_ADD_MEMBERSHIP,
                (const char *) &mreq, sizeof (mreq)) < 0) {
            DBG("Failed to add multicast membership: " << strerror(errno));
            socket.shutdown();
            return;
        }
    }

    startThread();
}

void AnanasServer::TimestampListener::run()
{
    while (!threadShouldExit()) {
        if (socket.waitUntilReady(true, 500)) {
            uint8_t buffer[1500];
            juce::String senderIP;
            int senderPort;

            const auto bytesRead{socket.read(buffer, 1500, false, senderIP, senderPort)};

            if (bytesRead > 0) {
                // DBG("Received " << bytesRead << " bytes from " << senderIP << ":" << senderPort);

                // Check for Follow_Up message (0x08)
                if ((buffer[0] & 0x0f) == 0x08) {
                    DBG("Follow-up message found");

                    timespec ts{};

                    // Extract seconds (6 bytes)
                    for (int i = 0; i < 6; i++) {
                        ts.tv_sec = ts.tv_sec << 8 | buffer[34 + i];
                    }

                    // Extract nanoseconds (4 bytes)
                    for (int i = 0; i < 4; i++) {
                        ts.tv_nsec = ts.tv_nsec << 8 | buffer[40 + i];
                    }

                    DBG("Timestamp: " << ts.tv_sec << "." << ts.tv_nsec << " --- " << ctime(&ts.tv_sec));

                    if (onTimestamp != nullptr) {
                        onTimestamp(ts);
                    }

                    break;
                }
            } else if (bytesRead < 0) {
                DBG("Error receiving data: " << strerror(errno));
            }
        }
    }

    DBG("Stopping timestamp listener thread");
}

#include "AnanasServer.h"

AnanasServer::AnanasServer() : sender(fifo)
{
}

void AnanasServer::prepareToPlay(const int samplesPerBlockExpected, const double sampleRate)
{
    sender.prepare(samplesPerBlockExpected, sampleRate);
}

void AnanasServer::releaseResources()
{
    if (sender.isThreadRunning()) {
        sender.stopThread(1000);
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
        // Maybe clear the packet
        // packet.fillWith(0);
    }

    DBG("Stopping send thread");
}

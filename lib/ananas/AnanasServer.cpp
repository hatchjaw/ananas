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

// int enable_hwtstamp(int sock, const char* ifname) {
//     struct ifreq ifr;
//     struct hwtstamp_config hwconfig;
//
//     memset(&ifr, 0, sizeof(ifr));
//     strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);
//
//     memset(&hwconfig, 0, sizeof(hwconfig));
//     hwconfig.flags = 0; // No special flags
//     hwconfig.tx_type = HWTSTAMP_TX_ON;
//     hwconfig.rx_filter = HWTSTAMP_FILTER_ALL;
//
//     ifr.ifr_data = (caddr_t)&hwconfig;
//
//     if (ioctl(sock, SIOCSHWTSTAMP, &ifr) < 0) {
//         perror("SIOCSHWTSTAMP ioctl failed");
//         return -1;
//     }
//
//     return 0; // Success
// }
//
// void enable_socket_timestamping(int sock) {
//     int flags = SOF_TIMESTAMPING_TX_HARDWARE |   // Request hardware TX timestamps
//                 SOF_TIMESTAMPING_RX_HARDWARE |   // Request hardware RX timestamps
//                 SOF_TIMESTAMPING_SOFTWARE |      // Fallback to software timestamps
//                 SOF_TIMESTAMPING_RAW_HARDWARE;   // Receive raw hardware timestamps
//
//     if (setsockopt(sock, SOL_SOCKET, SO_TIMESTAMPING, &flags, sizeof(flags)) < 0) {
//         perror("setsockopt(SO_TIMESTAMPING) failed");
//     }
// }


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

        // enable_hwtstamp(socket.getRawSocketHandle(), "enp85s0u2u3u1");
        // enable_socket_timestamping(socket.getRawSocketHandle());
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

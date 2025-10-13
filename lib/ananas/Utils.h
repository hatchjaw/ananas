#ifndef UTILS_H
#define UTILS_H

#include <juce_core/juce_core.h>

namespace ananas
{
    class Constants
    {
    public:
        constexpr static int64_t NSPS{1'000'000'000};
        constexpr static size_t FramesPerPacket{32};
        /**
         * Tweak this value such that clients stay in the middle of their
         * packet buffer.
         * Increase the divider if clients are reporting a lot of available
         * packets; decrease it if they're reporting too few.
         */
        constexpr static int64_t PacketOffsetNs{NSPS / 60};
        constexpr static size_t ClientPacketBufferSize{50};

        constexpr static uint16_t FifoCapacityFrames{(1 << 10)};
        constexpr static int FifoReportIntervalMs{2000};

        inline const static juce::StringRef LocalInterfaceIP{"192.168.10.10"};

        inline const static juce::StringRef SenderThreadName{"Ananas Sender"};
        constexpr static int SenderSocketLocalPort{14841};
        constexpr static int SenderSocketRemotePort{49152};
        inline const static juce::StringRef SenderSocketMulticastIP{"224.4.224.4"};

        inline const static juce::StringRef TimestampListenerThreadName{"Ananas Timestamp Listener"};
        constexpr static int TimestampListenerLocalPort{320};
        inline const static juce::StringRef PTPMulticastIP{"224.0.1.129"};
        constexpr static int TimestampListenerSocketTimeoutMs{5000};
        constexpr static int TimestampListenerBufferSize{1500};
        constexpr static int FollowUpMessageType{0x08};

        inline const static juce::StringRef ClientListenerThreadName{"Ananas Client Listener"};
        constexpr static int ClientListenerLocalPort{49153};
        constexpr static int ClientListenerSocketTimeoutMs{5000};
        constexpr static int ClientListenerBufferSize{1500};
        constexpr static int ClientConnectednessCheckIntervalMs{1000};
        constexpr static uint32_t ClientDisconnectedThresholdMs{1000};
        inline static const juce::Identifier ConnectedClientsParamID{"ConnectedClients"};
    };
}

#endif //UTILS_H

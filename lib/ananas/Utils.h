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
         * NSPS/8 for packets of 128 frames.
         * NSPS/16 for packets of 64 frames.
         * NSPS/32 for packets of 32 frames.
         * NSPS/64 for packets of 16 frames.
         */
        constexpr static int64_t PacketOffsetNs{NSPS / 32};
        constexpr static size_t ClientPacketBufferSize{50};

        constexpr static uint16_t FifoCapacityFrames{(1 << 10)};
        constexpr static int FifoReportIntervalMs{2000};

        inline const static juce::StringRef LocalInterfaceIP{"192.168.10.10"};

        inline const static juce::StringRef SenderThreadName{"Ananas Sender"};
        constexpr static int SenderSocketLocalPort{14841};
        constexpr static int SenderSocketRemotePort{49152};
        inline const static juce::StringRef SenderSocketMulticastIP{"224.4.224.4"};

        inline const static juce::StringRef TimestampListenerThreadName{"Ananas Timestamp Listener"};
        constexpr static int TimestapListenerLocalPort{320};
        inline const static juce::StringRef PTPMulticastIP{"224.0.1.129"};
        constexpr static int TimestampListenerSocketTimeoutMs{500};
        constexpr static int TimestampListenerBufferSize{1500};
        constexpr static int FollowUpMessageType{0x08};
    };
}

#endif //UTILS_H

#ifndef ANANASUTILS_H
#define ANANASUTILS_H

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
        inline const static juce::StringRef AudioMulticastIP{"224.4.224.4"};
        constexpr static int SenderSocketTimeoutMs{500};

        inline const static juce::StringRef ControlMulticastIP{"224.4.224.5"};

        inline const static juce::StringRef TimestampListenerThreadName{"Ananas Timestamp Listener"};
        constexpr static int TimestampListenerLocalPort{320};
        inline const static juce::StringRef PTPMulticastIP{"224.0.1.129"};
        constexpr static int TimestampListenerSocketTimeoutMs{1500};
        constexpr static int TimestampListenerBufferSize{1500};
        constexpr static int FollowUpMessageType{0x08};

        inline const static juce::StringRef ClientListenerThreadName{"Ananas Client Listener"};
        constexpr static int ClientListenerLocalPort{49153};
        inline const static juce::StringRef ClientAnnounceMulticastIP{"224.4.224.6"};
        constexpr static int ClientListenerSocketTimeoutMs{500};
        constexpr static int ClientListenerBufferSize{1500};
        constexpr static int ClientConnectednessCheckIntervalMs{1000};
        constexpr static juce::uint32 ClientDisconnectedThresholdMs{1000};

        inline const static juce::StringRef AuthorityListenerThreadName{"Ananas Authority Listener"};
        constexpr static int AuthorityListenerLocalPort{49154};
        inline const static juce::StringRef AuthorityAnnounceMulticastIP{"224.4.224.7"};
        constexpr static int AuthorityListenerSocketTimeoutMs{500};
        constexpr static int AuthorityListenerBufferSize{1500};
        constexpr static int AuthorityConnectednessCheckIntervalMs{1000};
        constexpr static juce::uint32 AuthorityDisconnectedThresholdMs{1000};
        constexpr static juce::int32 AuthorityInitialUSBFeedbackAccumulator{48 << 25};

        inline static const juce::StringRef SwitchInspectorThreadName{"Ananas Switch Inspector"};
        constexpr static juce::uint32 SwitchInspectorRequestTimeoutMs{1000};
    };

    class Identifiers
    {
    public:
        inline static const juce::Identifier ConnectedClientsParamID{"ConnectedClients"};

        inline const static juce::Identifier ClientSerialNumberPropertyID{"serialNumber"};
        inline const static juce::Identifier ClientPTPLockPropertyID{"ptpLock"};
        inline const static juce::Identifier ClientPresentationTimeOffsetNsPropertyID{"presentationTimeOffsetNs"};
        inline const static juce::Identifier ClientPresentationTimeOffsetFramePropertyID{"presentationTimeOffsetFrame"};
        inline const static juce::Identifier ClientAudioPTPOffsetPropertyID{"AudioPTPOffsetNs"};
        inline const static juce::Identifier ClientBufferFillPercentPropertyID{"bufferFillPercent"};
        inline const static juce::Identifier ClientSamplingRatePropertyID{"samplingRate"};
        inline const static juce::Identifier ClientPercentCPUPropertyID{"percentCPU"};

        inline static const juce::Identifier TimeAuthorityParamID{"TimeAuthority"};

        inline const static juce::Identifier AuthorityIpPropertyID{"ipAddress"};
        inline const static juce::Identifier AuthoritySerialNumberPropertyID{"serialNumber"};
        inline const static juce::Identifier AuthorityFeedbackAccumulatorPropertyID{"feedbackAccumulator"};

        inline const static juce::Identifier SwitchesParamID{"Switches"};

        inline const static juce::Identifier SwitchIpPropertyID{"switchIP"};
        inline const static juce::Identifier SwitchUsernamePropertyID{"switchUsername"};
        inline const static juce::Identifier SwitchPasswordPropertyID{"switchPassword"};
        inline const static juce::Identifier SwitchShouldRemovePropertyID{"switchShouldRemove"};

        inline const static juce::Identifier SwitchClockIdPropertyId{"clock-id"};
        inline const static juce::Identifier SwitchFreqDriftPropertyId{"freq-drift"};
        inline const static juce::Identifier SwitchGmClockIdPropertyId{"gm-clock-id"};
        inline const static juce::Identifier SwitchGmPriority1PropertyId{"gm-priority1"};
        inline const static juce::Identifier SwitchGmPriority2PropertyId{"gm-priority2"};
        inline const static juce::Identifier SwitchIAmGmPropertyId{"i-am-gm"};
        inline const static juce::Identifier SwitchMasterClockIdPropertyId{"master-clock-id"};
        inline const static juce::Identifier SwitchNamePropertyId{"name"};
        inline const static juce::Identifier SwitchOffsetPropertyId{"offset"};
        inline const static juce::Identifier SwitchPriority1PropertyId{"priority1"};
        inline const static juce::Identifier SwitchPriority2PropertyId{"priority2"};
        inline const static juce::Identifier SwitchSlavePortPropertyId{"slave-port"};
        inline const static juce::Identifier SwitchSlavePortDelayPropertyId{"slave-port-delay"};
    };
}

#endif //ANANASUTILS_H

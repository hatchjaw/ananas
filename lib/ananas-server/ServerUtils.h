#ifndef SERVERUTILS_H
#define SERVERUTILS_H

#include <juce_core/juce_core.h>

namespace ananas::Server
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
        constexpr static int64_t PacketOffsetNs{NSPS / 62};
        constexpr static size_t ClientPacketBufferSize{50};

        constexpr static uint16_t FifoCapacityFrames{(1 << 10)};
        constexpr static int FifoReportIntervalMs{2000};

        // TODO: make the local interface IP an option?
        inline const static juce::StringRef LocalInterfaceIP{"192.168.10.10"};

        inline const static juce::StringRef ControlMulticastIP{"224.4.224.5"};

        constexpr static size_t ListenerBufferSize{1500};

        constexpr static int PTPFollowUpMessageType{0x08};

        constexpr static int ClientConnectednessCheckIntervalMs{1000};

        constexpr static int AuthorityConnectednessCheckIntervalMs{1000};

        constexpr static int SwitchInspectorRequestTimeoutS{1};
        inline static const juce::StringRef SwitchMonitorPtpPath{"/rest/system/ptp/monitor"};
        inline static const juce::StringRef SwitchDisablePtpPath{"/rest/system/ptp/disable"};
        inline static const juce::StringRef SwitchEnablePtpPath{"/rest/system/ptp/enable"};

        constexpr static uint ThreadConnectSleepIntervalMs{2500};
        constexpr static uint ThreadConnectWaitIterations{ThreadConnectSleepIntervalMs / 100};
        constexpr static uint ThreadConnectWaitIntervalMs{ThreadConnectSleepIntervalMs / ThreadConnectWaitIterations};
    };

    struct ThreadParams
    {
        juce::StringRef name;
        int timeoutMs{};
    };

    struct ThreadSocket : ThreadParams
    {
        juce::StringRef ip;
        juce::uint16 localPort{};
    };

    struct SenderThreadSocket : ThreadSocket
    {
        juce::uint16 remotePort{};
    };

    struct ListenerThreadSocket : ThreadSocket
    {
        int disconnectionThresholdMs{};
    };

    class Threads
    {
    public:
        inline static const ThreadParams SwitchInspectorParams{
            "Ananas Switch Inspector",
            1100
        };
    };

    class Sockets
    {
    public:
        inline static const SenderThreadSocket AudioSenderSocket{
            "Ananas Audio Sender",
            500,
            "224.4.224.4",
            14841,
            49152
        };

        inline static const ListenerThreadSocket TimestampListenerSocket{
            "Ananas Timestamp Listener",
            1500,
            "224.0.1.129",
            320
        };

        inline static const ListenerThreadSocket ClientListenerSocket{
            "Ananas Client Listener",
            500,
            "224.4.224.6",
            49153,
            1250
        };

        inline static const ListenerThreadSocket AuthorityListenerSocket{
            "Ananas Authority Listener",
            500,
            "224.4.224.7",
            49154,
            1000
        };

        inline static const SenderThreadSocket RebootSenderSocket{
            "Ananas Reboot Sender",
            500,
            "224.4.224.8",
            14842,
            49155
        };
    };
}

#endif //SERVERUTILS_H

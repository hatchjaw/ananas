#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <juce_events/juce_events.h>

#include "Packet.h"

namespace ananas
{
    class ClientInfo
    {
    public:
        void update(const ClientAnnouncePacket *packet);

        [[nodiscard]] bool isConnected() const;

        [[nodiscard]] ClientAnnouncePacket getInfo() const;

    private:
        ClientAnnouncePacket info{};
        uint32_t lastReceiveTime{0};
    };

    class ClientList final : public juce::Timer,
                             public juce::ChangeBroadcaster
    {
    public:
        void handlePacket(const juce::String &clientIP, const ClientAnnouncePacket *packet);

        void timerCallback() override;

        [[nodiscard]] juce::var toVar() const;

        bool getShouldReboot() const;

        void setShouldReboot(bool should);

        bool shouldNotify();

        uint getCount() const;

    private:
        void checkConnectivity();

        std::map<juce::String, ClientInfo> clients;
        bool shouldReboot{false};
        bool clientJoined{false};
    };
}


#endif //CLIENTINFO_H

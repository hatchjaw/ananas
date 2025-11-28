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
        [[nodiscard]] juce::var toJSON() const;

        void handlePacket(const juce::String &clientIP, const ClientAnnouncePacket *packet);

        void timerCallback() override;

        juce::var toVar() const;

    private:
        void checkConnectivity();

        std::map<juce::String, ClientInfo> clients;
    };
}


#endif //CLIENTINFO_H

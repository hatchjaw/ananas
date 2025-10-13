#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <juce_events/juce_events.h>

#include "Packet.h"

namespace ananas
{
    class ClientInfo
    {
    public:
        void update(const AnnouncementPacket *packet);

        [[nodiscard]] bool isConnected() const;

        [[nodiscard]] AnnouncementPacket getInfo() const;

    private:
        AnnouncementPacket info{};
        uint32_t lastReceiveTime{0};
    };

    class ClientList final : public juce::Timer,
                             public juce::ChangeBroadcaster
    {
    public:
        [[nodiscard]] juce::var toJSON() const;

        void handlePacket(const juce::String &clientIP, const AnnouncementPacket *packet);

        void timerCallback() override;

        juce::var toVar() const;

    private:
        std::map<juce::String, ClientInfo> clients;
    };
}


#endif //CLIENTINFO_H

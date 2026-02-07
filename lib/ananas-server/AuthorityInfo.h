#ifndef AUTHORITYINFO_H
#define AUTHORITYINFO_H

#include <Packet.h>
#include <juce_events/juce_events.h>

namespace ananas
{
    class AuthorityInfo final : public juce::ChangeBroadcaster
    {
    public:
        void handlePacket(const juce::String &senderIP, AuthorityAnnouncePacket *packet);

        juce::var toVar() const;

    private:
        juce::String ip{""};
        AuthorityAnnouncePacket info{};
    };
}


#endif //AUTHORITYINFO_H

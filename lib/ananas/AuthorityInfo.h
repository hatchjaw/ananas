#ifndef AUTHORITYINFO_H
#define AUTHORITYINFO_H

#include <Packet.h>
#include <juce_events/juce_events.h>

namespace ananas
{
    class AuthorityInfo final : public juce::ChangeBroadcaster
    {
    public:
        void update(const juce::String &authorityIP, const AuthorityAnnouncePacket *packet);

        juce::String &getIP();

        AuthorityAnnouncePacket &getInfo();

        juce::var toVar() const;

    private:
        juce::String ip{""};
        AuthorityAnnouncePacket info{};
    };
}


#endif //AUTHORITYINFO_H

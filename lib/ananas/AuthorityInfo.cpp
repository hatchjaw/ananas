#include "AuthorityInfo.h"
#include "AnanasUtils.h"

namespace ananas
{
    void AuthorityInfo::update() //juce::String &authorityIP, const AuthorityAnnouncePacket *packet)
    {
        // ip = authorityIP;
        // info = *packet;
        sendChangeMessage();
    }

    juce::String &AuthorityInfo::getIP()
    {
        return ip;
    }

    AuthorityAnnouncePacket &AuthorityInfo::getInfo()
    {
        return info;
    }

    juce::var AuthorityInfo::toVar() const
    {
        const auto object{new juce::DynamicObject()};

        object->setProperty(Identifiers::AuthorityIpPropertyID, ip);
        object->setProperty(Identifiers::AuthoritySerialNumberPropertyID, static_cast<int>(info.serial));
        object->setProperty(Identifiers::AuthorityFeedbackAccumulatorPropertyID, static_cast<int>(info.usbFeedbackAccumulator));

        return object;
    }
}

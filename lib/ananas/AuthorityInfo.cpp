#include "AuthorityInfo.h"
#include "AnanasUtils.h"

void ananas::AuthorityInfo::update(const juce::String &authorityIP, const AuthorityAnnouncePacket *packet)
{
    ip = authorityIP;
    info = *packet;
    sendChangeMessage();
}

juce::String &ananas::AuthorityInfo::getIP()
{
    return ip;
}

ananas::AuthorityAnnouncePacket &ananas::AuthorityInfo::getInfo()
{
    return info;
}

juce::var ananas::AuthorityInfo::toVar() const
{
    const auto object{new juce::DynamicObject()};

    object->setProperty(Identifiers::AuthorityIpPropertyID, ip);
    object->setProperty(Identifiers::AuthoritySerialNumberPropertyID, static_cast<int>(info.serial));
    object->setProperty(Identifiers::AuthorityFeedbackAccumulatorPropertyID, static_cast<int>(info.usbFeedbackAccumulator));

    return object;
}

#include "AuthorityInfo.h"
#include <AnanasUtils.h>

namespace ananas
{
    void AuthorityInfo::handlePacket(const juce::String &senderIP, AuthorityAnnouncePacket *packet)
    {
        ip = senderIP;
        info = *packet;
        sendChangeMessage();
    }

    juce::var AuthorityInfo::toVar() const
    {
        const auto object{new juce::DynamicObject()};

        object->setProperty(Utils::Identifiers::AuthorityIpPropertyID, ip);
        object->setProperty(Utils::Identifiers::AuthoritySerialNumberPropertyID, static_cast<int>(info.serial));
        object->setProperty(Utils::Identifiers::AuthorityFeedbackAccumulatorPropertyID, static_cast<int>(info.usbFeedbackAccumulator));

        return object;
    }
}

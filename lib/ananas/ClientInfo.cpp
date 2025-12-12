#include "ClientInfo.h"
#include "AnanasUtils.h"

namespace ananas
{
    void ClientInfo::update(const ClientAnnouncePacket *packet)
    {
        lastReceiveTime = juce::Time::getMillisecondCounter();
        info = *packet;
    }

    bool ClientInfo::isConnected() const
    {
        return juce::Time::getMillisecondCounter() - lastReceiveTime < Constants::ClientDisconnectedThresholdMs;
    }

    ClientAnnouncePacket ClientInfo::getInfo() const
    {
        return info;
    }

    //==============================================================================

    void ClientList::handlePacket(const juce::String &clientIP, const ClientAnnouncePacket *packet)
    {
        if (clients.empty()) {
            startTimer(Constants::ClientConnectednessCheckIntervalMs);
        }

        auto iter{clients.find(clientIP)};
        if (iter == clients.end()) {
            ClientInfo c{};
            iter = clients.insert(std::make_pair(clientIP, c)).first;
            std::cout << "Client " << iter->first << " connected." << std::endl;
        }
        iter->second.update(packet);

        sendChangeMessage();
    }

    void ClientList::timerCallback()
    {
        checkConnectivity();
    }

    juce::var ClientList::toVar() const
    {
        const auto object{new juce::DynamicObject()};

        for (const auto &[ip, clientInfo]: clients) {
            auto *client{new juce::DynamicObject()};
            const auto &info{clientInfo.getInfo()};
            client->setProperty(Identifiers::ClientSerialNumberPropertyID, static_cast<int>(info.serial));
            client->setProperty(Identifiers::ClientPTPLockPropertyID, info.ptpLock);
            client->setProperty(Identifiers::ClientPresentationTimeOffsetNsPropertyID, info.presentationOffsetNs);
            client->setProperty(Identifiers::ClientPresentationTimeOffsetFramePropertyID, info.presentationOffsetFrame);
            client->setProperty(Identifiers::ClientAudioPTPOffsetPropertyID, info.audioPTPOffsetNs);
            client->setProperty(Identifiers::ClientBufferFillPercentPropertyID, info.bufferFillPercent);
            client->setProperty(Identifiers::ClientSamplingRatePropertyID, info.samplingRate);
            client->setProperty(Identifiers::ClientPercentCPUPropertyID, info.percentCPU);
            object->setProperty(ip, client);
        }

        return object;
    }

    bool ClientList::getShouldReboot() const
    {
        return shouldReboot;
    }

    void ClientList::setShouldReboot(const bool should)
    {
        shouldReboot = should;
    }

    void ClientList::checkConnectivity()
    {
        for (auto it{clients.begin()}, next{it}; it != clients.end(); it = next) {
            ++next;
            if (!it->second.isConnected()) {
                std::cout << "Client " << it->first << " disconnected." << std::endl;
                clients.erase(it);
                sendChangeMessage();
            }
        }
    }
}

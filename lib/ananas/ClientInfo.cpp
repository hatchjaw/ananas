#include "ClientInfo.h"
#include "Utils.h"

void ananas::ClientInfo::update(const AnnouncementPacket *packet)
{
    lastReceiveTime = juce::Time::getMillisecondCounter();
    info = *packet;
}

bool ananas::ClientInfo::isConnected() const
{
    return juce::Time::getMillisecondCounter() - lastReceiveTime < Constants::ClientDisconnectedThresholdMs;
}

ananas::AnnouncementPacket ananas::ClientInfo::getInfo() const
{
    return info;
}

//==============================================================================

juce::var ananas::ClientList::toJSON() const
{
    juce::var result = juce::var();

    for (auto it = clients.begin(); it != clients.end(); ++it) {
        result.append(it->first);
    }

    return juce::JSON::toString(result);
}

void ananas::ClientList::handlePacket(const juce::String &clientIP, const AnnouncementPacket *packet)
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

void ananas::ClientList::timerCallback()
{
    for (auto it{clients.begin()}, next{it}; it != clients.end(); it = next) {
        ++next;
        if (!it->second.isConnected()) {
            std::cout << "Client " << it->first << " disconnected." << std::endl;
            clients.erase(it);
            sendChangeMessage();
        } //else {
        //     auto info{it->second.getInfo()};
        //     std::cout <<
        //             "Client " << it->first <<
        //             ", serial " << info.serial <<
        //             ", offset " << info.offsetTime <<
        //             " ns (" << info.offsetFrame << " frames)" <<
        //             ", packet buffer " << static_cast<int>(info.bufferFillPercent) << " % full" <<
        //             ", sampling rate " << std::fixed << std::setprecision(6) << info.samplingRate <<
        //             std::defaultfloat << ", " << info.percentCPU << " % CPU" <<
        //             std::endl;
        // }
    }
}

juce::var ananas::ClientList::toVar() const
{
    auto object{juce::var()};

    for (const auto& [ip, clientInfo] : clients) {
        auto* client{new juce::DynamicObject()};
        const auto& info{clientInfo.getInfo()};
        client->setProperty("ip", ip);
        client->setProperty("serial", static_cast<int>(info.serial));
        client->setProperty("offsetTime", info.offsetTime);
        client->setProperty("offsetFrame", info.offsetFrame);
        client->setProperty("bufferFillPercent", info.bufferFillPercent);
        client->setProperty("samplingRate", info.samplingRate);
        client->setProperty("percentCPU", info.percentCPU);
        object.append(client);
    }

    return object;
}

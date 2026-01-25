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

    //==========================================================================

    void ModuleInfo::update()
    {
        lastReceiveTime = juce::Time::getMillisecondCounter();
    }

    uint ModuleInfo::getModuleId() const
    {
        return id;
    }

    void ModuleInfo::setModuleId(const uint id)
    {
        this->id = id;
    }

    juce::ValueTree ModuleInfo::toValueTree() const
    {
        juce::ValueTree tree{"Module"};
        tree.setProperty(Identifiers::ModuleIDPropertyID, static_cast<int>(id), nullptr);
        return tree;
    }

    bool ModuleInfo::isConnected() const
    {
        return juce::Time::getMillisecondCounter() - lastReceiveTime < Constants::ClientDisconnectedThresholdMs;
    }

    bool ModuleInfo::justDisconnected()
    {
        const auto didJustDisconnect{!isConnected() && wasConnected};

        if (didJustDisconnect) {
            wasConnected = false;
        }

        return didJustDisconnect;
    }

    bool ModuleInfo::justConnected()
    {
        const auto didJustConnect{isConnected() && !wasConnected};

        if (didJustConnect) {
            wasConnected = true;
        }

        return didJustConnect;
    }

    ModuleInfo ModuleInfo::fromValueTree(const juce::ValueTree &tree)
    {
        ModuleInfo info;
        if (const auto &prop{tree.getProperty(Identifiers::ModuleIDPropertyID)}; prop.isInt()) {
            const int val{prop};
            info.id = static_cast<uint>(val);
        }
        return info;
    }

    //==========================================================================

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
            client->setProperty(Identifiers::ClientModuleIDPropertyID, info.moduleID);
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

    uint ClientList::getCount() const
    {
        return clients.size();
    }

    juce::ValueTree ClientList::toValueTree()
    {
        juce::ValueTree tree(Identifiers::ConnectedClientsParamID);

        for (const auto &[ip, _]: clients) {
            juce::ValueTree subTree("Client");
            subTree.setProperty("ip", ip, nullptr);
            tree.addChild(subTree, -1, nullptr);
        }

        return tree;
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

    //==========================================================================

    void ModuleList::handlePacket(const juce::String &moduleIP)
    {
        if (modules.empty()) {
            startTimer(Constants::ClientConnectednessCheckIntervalMs);
        }

        auto iter{modules.find(moduleIP)};
        if (iter == modules.end()) {
            ModuleInfo m{};
            iter = modules.insert(std::make_pair(moduleIP, m)).first;
        }
        iter->second.update();
        if (iter->second.justConnected()) {
            sendChangeMessage();
        }
    }

    void ModuleList::timerCallback()
    {
        checkConnectivity();
    }

    juce::var ModuleList::toVar() const
    {
        const auto object{new juce::DynamicObject()};

        for (const auto &[ip, m]: modules) {
            auto *module{new juce::DynamicObject()};
            module->setProperty(Identifiers::ModuleIDPropertyID, static_cast<int>(m.getModuleId()));
            module->setProperty(Identifiers::ModuleIsConnectedPropertyID, m.isConnected());
            object->setProperty(ip, module);
        }

        return object;
    }

    juce::ValueTree ModuleList::toValueTree() const
    {
        juce::ValueTree tree(Identifiers::SwitchesParamID);

        for (const auto &[ip, m]: modules) {
            auto moduleTree{m.toValueTree()};
            moduleTree.setProperty("ip", ip, nullptr);
            tree.addChild(moduleTree, -1, nullptr);
        }

        return tree;
    }

    void ModuleList::fromValueTree(const juce::ValueTree &tree)
    {
        modules.clear();

        for (int i{0}; i < tree.getNumChildren(); ++i) {
            auto moduleTree{tree.getChild(i)};
            juce::Identifier ip{moduleTree.getProperty("ip")};
            modules[ip.toString()] = ModuleInfo::fromValueTree(moduleTree);
        }
    }

    void ModuleList::checkConnectivity()
    {
        for (auto it{modules.begin()}; it != modules.end(); ++it) {
            if (it->second.justDisconnected()) {
                sendChangeMessage();
            }
        }
    }
}

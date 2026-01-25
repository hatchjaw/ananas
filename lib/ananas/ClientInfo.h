#ifndef CLIENTINFO_H
#define CLIENTINFO_H

#include <juce_events/juce_events.h>
#include <juce_data_structures/juce_data_structures.h>
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

    class ModuleInfo
    {
    public:
        void update();


        [[nodiscard]] uint getModuleId() const;

        void setModuleId(uint id);

        [[nodiscard]] juce::ValueTree toValueTree() const;

        [[nodiscard]] bool isConnected() const;

        [[nodiscard]] bool justDisconnected();

        [[nodiscard]] bool justConnected();

        static ModuleInfo fromValueTree(const juce::ValueTree &tree);

    private:
        uint id{0};
        juce::uint32 lastReceiveTime{0};
        bool wasConnected{false};
    };

    class ClientList final : public juce::Timer,
                             public juce::ChangeBroadcaster
    {
    public:
        void handlePacket(const juce::String &clientIP, const ClientAnnouncePacket *packet);

        void timerCallback() override;

        [[nodiscard]] juce::var toVar() const;

        [[nodiscard]] bool getShouldReboot() const;

        void setShouldReboot(bool should);

        [[nodiscard]] uint getCount() const;

        juce::ValueTree toValueTree();

    private:
        void checkConnectivity();

        std::map<juce::String, ClientInfo> clients;
        bool shouldReboot{false};
    };

    class ModuleList final : public juce::ChangeBroadcaster,
                             public juce::Timer
    {
    public:
        void handlePacket(const juce::String &moduleIP);

        void timerCallback() override;

        [[nodiscard]] juce::var toVar() const;

        [[nodiscard]] juce::ValueTree toValueTree() const;

        void fromValueTree(const juce::ValueTree &tree);

    private:
        void checkConnectivity();

        std::map<juce::String, ModuleInfo> modules;
    };
}


#endif //CLIENTINFO_H

#ifndef SWITCHINFO_H
#define SWITCHINFO_H

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

namespace ananas
{
    class SwitchInfo
    {
    public:
        void update(const juce::var *response);

        juce::var toVar() const;

        juce::String ip;
        juce::String username;
        juce::String password;
    private:
        juce::var info;

        juce::String clockID;
        juce::int64 freqDrift{0};
        juce::String gmClockID;
        uint8_t gmPriority1{0};
        uint8_t gmPriority2{0};
        bool iAmGm{false};
        juce::String masterClockID;
        juce::String name;
        juce::int64 offset{0};
        uint8_t priority1{0};
        uint8_t priority2{0};
        juce::String slavePort;
        uint64_t slavePortDelay{0};
    };

    class SwitchList final : public juce::ChangeBroadcaster
    {
    public:
        void handleEdit(const juce::var &data);

        void handleResponse(const int switchIndex, const juce::var &response);

        [[nodiscard]] juce::var toVar() const;

    private:
        // std::map<juce::String, SwitchInfo> switches;
        std::map<int, SwitchInfo> switches;
    };
}


#endif //SWITCHINFO_H

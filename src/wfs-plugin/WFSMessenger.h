#ifndef WFSMESSENGER_H
#define WFSMESSENGER_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_osc/juce_osc.h>

namespace ananas::WFS
{
    class WFSMessenger final : public juce::OSCSender,
                               public juce::AudioProcessorValueTreeState::Listener,
                               public juce::ValueTree::Listener
    {
    public:
        WFSMessenger()
        {
        }

        bool prepare();

        void valueTreePropertyChanged(::juce::ValueTree &treeWhosePropertyHasChanged, const ::juce::Identifier &property) override;

        void parameterChanged(const juce::String &parameterID, float newValue) override;

    private:
        juce::DatagramSocket socket;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WFSMessenger);
    };
} // ananas::WFS

#endif //WFSMESSENGER_H

#ifndef PLUGINEDITOR_H
#define PLUGINEDITOR_H

#include "PluginProcessor.h"


class PluginEditor final : public juce::AudioProcessorEditor,
                           public juce::AudioProcessorValueTreeState::Listener,
                           public juce::ValueTree::Listener,
                           juce::AsyncUpdater
{
public:
    explicit PluginEditor(PluginProcessor &);

    ~PluginEditor() override;

    void paint(juce::Graphics &) override;

    void resized() override;

    void parameterChanged(const juce::String &parameterID, float newValue) override;

    void valueTreePropertyChanged(juce::ValueTree &tree, const juce::Identifier &property) override;

    void valueTreeChildAdded(juce::ValueTree &parent,
                             juce::ValueTree &child) override
    {
        ignoreUnused(parent, child);
        DBG("valueTreeChildAdded");
    }

    void valueTreeChildRemoved(juce::ValueTree &parent,
                               juce::ValueTree &child, int) override
    {
        ignoreUnused(parent, child);
        DBG("valueTreeChildRemoved");
    }

    void valueTreeChildOrderChanged(juce::ValueTree &parent,
                                    int oldIndex, int newIndex) override
    {
        ignoreUnused(parent, oldIndex, newIndex);
        DBG("valueTreeChildOrderChanged");
    }

    void valueTreeParentChanged(juce::ValueTree &tree) override
    {
        ignoreUnused(tree);
        DBG("valueTreeParentChanged");
    }

    void handleAsyncUpdate() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)

    PluginProcessor &getProcessor();

    const PluginProcessor &getProcessor() const;
};


#endif //PLUGINEDITOR_H

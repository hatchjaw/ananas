#ifndef PLUGINEDITOR_H
#define PLUGINEDITOR_H

#include "PluginProcessor.h"


class PluginEditor final : public juce::AudioProcessorEditor
{
public:
    PluginEditor(PluginProcessor &,
                 juce::AudioProcessorValueTreeState &,
                 juce::ValueTree &);

    ~PluginEditor() override;

    void paint(juce::Graphics &) override;

    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)

    PluginProcessor &processor;
    juce::AudioProcessorValueTreeState &staticTree;
    juce::ValueTree &dynamicTree;
};


#endif //PLUGINEDITOR_H

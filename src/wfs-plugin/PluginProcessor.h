#ifndef PLUGINPROCESSOR_H
#define PLUGINPROCESSOR_H

#include <Server.h>
#include <juce_audio_processors/juce_audio_processors.h>


class PluginProcessor final : public juce::AudioProcessor
{
public:
    PluginProcessor();

    ~PluginProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    void releaseResources() override;

    void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override;

    juce::AudioProcessorEditor *createEditor() override;

    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;

    bool producesMidi() const override;

    bool isMidiEffect() const override;

    double getTailLengthSeconds() const override;

    int getNumPrograms() override;

    int getCurrentProgram() override;

    void setCurrentProgram(int index) override;

    const juce::String getProgramName(int index) override;

    void changeProgramName(int index, const juce::String &newName) override;

    void getStateInformation(juce::MemoryBlock &destData) override;

    void setStateInformation(const void *data, int size) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    ananas::Server server;

    // For handling parameters that are known at compile time.
    juce::AudioProcessorValueTreeState apvts;
    // For handling parameters that are not known until runtime.
    juce::ValueTree dynamicTree;
};


#endif //PLUGINPROCESSOR_H

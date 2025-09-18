#include "PluginProcessor.h"

#include "PluginEditor.h"
#include "Utils.h"

PluginProcessor::PluginProcessor()
    : apvts(*this, nullptr, "WFS Parameters", createParameterLayout()),
      dynamicTree("Module Parameters")
{
}

PluginProcessor::~PluginProcessor()
{
    server.releaseResources(); // Why the bloody hell doesn't this work?
}

void PluginProcessor::prepareToPlay(const double sampleRate, const int samplesPerBlock)
{
    server.prepareToPlay(samplesPerBlock, sampleRate);
}

void PluginProcessor::releaseResources()
{
    server.releaseResources();
}

void PluginProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    juce::AudioSourceChannelInfo block{buffer};

    server.getNextAudioBlock(block);
}

juce::AudioProcessorEditor *PluginProcessor::createEditor()
{
    return new PluginEditor(*this, apvts, dynamicTree);
}

bool PluginProcessor::hasEditor() const
{
    return true;
}

const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String PluginProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void PluginProcessor::changeProgramName(int index, const juce::String &newName)
{
    ignoreUnused(index, newName);
}

void PluginProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    ignoreUnused(destData);
}

void PluginProcessor::setStateInformation(const void *data, int size)
{
    juce::ignoreUnused(data, size);
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params{};

    params.add(std::make_unique<juce::AudioParameterFloat>(
            ananas::WFS::Utils::speakerSpacingParamID,
            "Speaker Spacing (m)",
            juce::NormalisableRange{.05f, .4f, .001f},
            .2f
    ));

    return params;
}

//==============================================================================
// This creates new instances of the plugin.
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
    return new PluginProcessor();
}

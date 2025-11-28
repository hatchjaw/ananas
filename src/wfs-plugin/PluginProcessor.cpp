#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(getBusesProperties()),
      server(std::make_unique<ananas::Server>(ananas::WFS::Constants::MaxChannelsToSend)),
      apvts(*this, nullptr, ananas::WFS::Identifiers::StaticTreeType, createParameterLayout()),
      dynamicTree(ananas::WFS::Identifiers::DynamicTreeType)
{
    server->getClientList()->addChangeListener(this);
    server->getAuthority()->addChangeListener(this);
}

PluginProcessor::~PluginProcessor()
{
    server->getClientList()->removeChangeListener(this);
    server->getAuthority()->removeChangeListener(this);
    // dynamicTree.removeListener(wfsMessenger.get());
}

void PluginProcessor::prepareToPlay(const double sampleRate, const int samplesPerBlock)
{
    server->prepareToPlay(samplesPerBlock, sampleRate);
}

void PluginProcessor::releaseResources()
{
    server->releaseResources();
}

void PluginProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    const juce::AudioSourceChannelInfo block{buffer};

    server->getNextAudioBlock(block);
}

void PluginProcessor::processBlock(juce::AudioBuffer<double> &buffer, juce::MidiBuffer &midiMessages)
{
    juce::AudioBuffer<float> floatBuffer;

    floatBuffer.makeCopyOf(buffer);

    processBlock(floatBuffer, midiMessages);
}

juce::AudioProcessorEditor *PluginProcessor::createEditor()
{
    return new PluginEditor(*this);
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

void PluginProcessor::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    if (const auto *clients = dynamic_cast<ananas::ClientList *>(source)) {
        dynamicTree.setProperty(ananas::Constants::ConnectedClientsParamID, clients->toVar(), nullptr);
    } else if (const auto *authority = dynamic_cast<ananas::AuthorityInfo *>(source)) {
        dynamicTree.setProperty(ananas::Constants::TimeAuthorityParamID, authority->toVar(), nullptr);
    }
}

juce::ValueTree &PluginProcessor::getDynamicTree()
{
    return dynamicTree;
}

const juce::ValueTree &PluginProcessor::getDynamicTree() const
{
    return dynamicTree;
}

juce::AudioProcessor::BusesProperties PluginProcessor::getBusesProperties()
{
    BusesProperties buses;

    for (int i{0}; i < ananas::WFS::Constants::MaxChannelsToSend; ++i) {
        buses.addBus(true, ananas::WFS::Strings::InputLabel + juce::String{i + 1}, juce::AudioChannelSet::mono());
        buses.addBus(false, ananas::WFS::Strings::OutputLabel + juce::String{i + 1}, juce::AudioChannelSet::mono());
    }

    return buses;
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params{};

    params.add(std::make_unique<juce::AudioParameterFloat>(
        ananas::WFS::Params::SpeakerSpacing.id,
        ananas::WFS::Params::SpeakerSpacing.name,
        juce::NormalisableRange{.05f, .4f, .001f},
        .2f
    ));

    return params;
}

//==============================================================================
// This creates new instances of the plugin.
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

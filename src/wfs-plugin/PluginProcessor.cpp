#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Utils.h"

PluginProcessor::PluginProcessor()
    : AudioProcessor(getBusesProperties()),
      server(std::make_unique<ananas::Server>(ananas::WFS::Constants::MaxChannelsToSend)),
      apvts(*this, nullptr, ananas::WFS::Identifiers::StaticTreeType, createParameterLayout()),
      dynamicTree(ananas::WFS::Identifiers::DynamicTreeType),
      persistentTree(ananas::WFS::Identifiers::PersistentTreeType)
{
    server->getClientList()->addChangeListener(this);
    server->getModuleList()->addChangeListener(this);
    server->getAuthority()->addChangeListener(this);
    server->getSwitches()->addChangeListener(this);
    persistentTree.addListener(&wfsMessenger);
    apvts.addParameterListener(ananas::WFS::Params::SpeakerSpacing.id, &wfsMessenger);
    for (uint n{0}; n < ananas::WFS::Constants::MaxChannelsToSend; ++n) {
        apvts.addParameterListener(ananas::WFS::Params::getSourcePositionParamID(n, ananas::WFS::SourcePositionAxis::X), &wfsMessenger);
        apvts.addParameterListener(ananas::WFS::Params::getSourcePositionParamID(n, ananas::WFS::SourcePositionAxis::Y), &wfsMessenger);

        // Set up source amplitudes for visualisation...
        sourceAmplitudes.set(static_cast<int>(n), new std::atomic{0.f});
    }
}

PluginProcessor::~PluginProcessor()
{
    server->getClientList()->removeChangeListener(this);
    server->getModuleList()->removeChangeListener(this);
    server->getAuthority()->removeChangeListener(this);
    server->getSwitches()->removeChangeListener(this);
    persistentTree.removeListener(&wfsMessenger);
    apvts.removeParameterListener(ananas::WFS::Params::SpeakerSpacing.id, &wfsMessenger);
    for (uint n{0}; n < ananas::WFS::Constants::MaxChannelsToSend; ++n) {
        apvts.removeParameterListener(ananas::WFS::Params::getSourcePositionParamID(n, ananas::WFS::SourcePositionAxis::X), &wfsMessenger);
        apvts.removeParameterListener(ananas::WFS::Params::getSourcePositionParamID(n, ananas::WFS::SourcePositionAxis::Y), &wfsMessenger);
    }
    if (wfsMessenger.isThreadRunning()) {
        wfsMessenger.stopThread(ananas::WFS::Constants::WFSMessengerThreadTimeout);
    }
}

void PluginProcessor::prepareToPlay(const double sampleRate, const int samplesPerBlock)
{
    server->prepareToPlay(samplesPerBlock, sampleRate);
    wfsMessenger.startThread();
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

    // Store the RMS level for each channel for the current buffer.
    for (auto ch{0}; ch < buffer.getNumChannels(); ++ch) {
        sourceAmplitudes[ch]->store(juce::Decibels::gainToDecibels(buffer.getRMSLevel(ch, 0, buffer.getNumSamples())));
    }
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
    auto state{apvts.copyState()};

    state.addChild(getServer().getSwitches()->toValueTree(), -1, nullptr);

    state.addChild(getServer().getModuleList()->toValueTree(), -1, nullptr);

    const auto xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void *data, int size)
{
    const auto xmlState{getXmlFromBinary(data, size)};

    if (xmlState != nullptr) {
        const auto tree{juce::ValueTree::fromXml(*xmlState)};

        if (tree.isValid()) {
            apvts.replaceState(tree);

            const auto switchListTree{tree.getChildWithName(ananas::Identifiers::SwitchesParamID)};
            if (switchListTree.isValid()) {
                getServer().getSwitches()->fromValueTree(switchListTree);
            }

            const auto moduleListTree{tree.getChildWithName(ananas::Identifiers::ModulesParamID)};
            if (moduleListTree.isValid()) {
                getServer().getModuleList()->fromValueTree(moduleListTree);
            }
        }
    }
}

void PluginProcessor::changeListenerCallback(juce::ChangeBroadcaster *source)
{
    if (const auto *clients = dynamic_cast<ananas::ClientList *>(source)) {
        dynamicTree.setProperty(ananas::Identifiers::ConnectedClientsParamID, clients->toVar(), nullptr);
    } else if (const auto *modules = dynamic_cast<ananas::ModuleList *>(source)) {
        persistentTree.setProperty(ananas::Identifiers::ModulesParamID, modules->toVar(), nullptr);

        wfsMessenger.parameterChanged(
            ananas::WFS::Params::SpeakerSpacing.id,
            apvts.getRawParameterValue(ananas::WFS::Params::SpeakerSpacing.id)->load()
        );

        for (uint n{0}; n < ananas::WFS::Constants::MaxChannelsToSend; ++n) {
            auto idX{ananas::WFS::Params::getSourcePositionParamID(n, ananas::WFS::SourcePositionAxis::X)},
                    idY{ananas::WFS::Params::getSourcePositionParamID(n, ananas::WFS::SourcePositionAxis::Y)};
            wfsMessenger.parameterChanged(idX, apvts.getRawParameterValue(idX)->load());
            wfsMessenger.parameterChanged(idY, apvts.getRawParameterValue(idY)->load());
        }
    } else if (const auto *authority = dynamic_cast<ananas::AuthorityInfo *>(source)) {
        dynamicTree.setProperty(ananas::Identifiers::TimeAuthorityParamID, authority->toVar(), nullptr);
    } else if (const auto *switches = dynamic_cast<ananas::SwitchList *>(source)) {
        persistentTree.setProperty(ananas::Identifiers::SwitchesParamID, switches->toVar(), nullptr);
        dynamicTree.setProperty(ananas::Identifiers::SwitchesParamID, switches->toVar(), nullptr);
    }
}

juce::AudioProcessorValueTreeState &PluginProcessor::getParamState()
{
    return apvts;
}

const juce::AudioProcessorValueTreeState &PluginProcessor::getParamState() const
{
    return apvts;
}

juce::ValueTree &PluginProcessor::getDynamicTree()
{
    return dynamicTree;
}

const juce::ValueTree &PluginProcessor::getDynamicTree() const
{
    return dynamicTree;
}

juce::ValueTree &PluginProcessor::getPersistentTree()
{
    return persistentTree;
}

const juce::ValueTree &PluginProcessor::getPersistentTree() const
{
    return persistentTree;
}

ananas::Server &PluginProcessor::getServer() const
{
    return *server;
}

juce::HashMap<int, std::atomic<float> *> &PluginProcessor::getSourceAmplitudes()
{
    return sourceAmplitudes;
}

juce::AudioProcessor::BusesProperties PluginProcessor::getBusesProperties()
{
    BusesProperties buses;

    for (uint i{0}; i < ananas::WFS::Constants::MaxChannelsToSend; ++i) {
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
        ananas::WFS::Params::SpeakerSpacing.range,
        ananas::WFS::Params::SpeakerSpacing.defaultValue
    ));

    params.add(std::make_unique<juce::AudioParameterBool>(
        ananas::WFS::Params::ShowModuleSelectors.id,
        ananas::WFS::Params::ShowModuleSelectors.name,
        ananas::WFS::Params::ShowModuleSelectors.defaultValue
    ));

    for (uint n{0}; n < ananas::WFS::Constants::MaxChannelsToSend; ++n) {
        params.add(std::make_unique<juce::AudioParameterFloat>(
            ananas::WFS::Params::getSourcePositionParamID(n, ananas::WFS::SourcePositionAxis::X),
            ananas::WFS::Params::getSourcePositionParamName(n, ananas::WFS::SourcePositionAxis::X),
            ananas::WFS::Params::SourcePositionRange,
            ananas::WFS::Params::getSourcePositionDefaultX(n)
        ));
        params.add(std::make_unique<juce::AudioParameterFloat>(
            ananas::WFS::Params::getSourcePositionParamID(n, ananas::WFS::SourcePositionAxis::Y),
            ananas::WFS::Params::getSourcePositionParamName(n, ananas::WFS::SourcePositionAxis::Y),
            ananas::WFS::Params::SourcePositionRange,
            ananas::WFS::Params::SourcePositionDefaultY
        ));
    }

    return params;
}

//==============================================================================
// This creates new instances of the plugin.
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

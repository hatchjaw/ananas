#include "MainComponent.h"

#include <utility>

MainComponent::MainComponent(const juce::File &file)
{
    setAudioChannels(0, 2);

    auto setup{deviceManager.getAudioDeviceSetup()};
    // auto &deviceTypes{deviceManager.getAvailableDeviceTypes()};
    // for (const auto *type: deviceTypes) {
    //     auto typeName{type->getTypeName()};
    //     DBG(typeName);
    //     auto devices{type->getDeviceNames()};
    //     for (const auto &device: devices) {
    //         DBG("  " << device);
    //     }
    // }
    // deviceManager.setCurrentAudioDeviceType("JACK", true);
    setup.sampleRate = 48000;
    setup.bufferSize = 128;
    // setup.outputDeviceName = "Built-in Audio Analog Stereo";
    deviceManager.setAudioDeviceSetup(setup, true);

    formatManager.registerBasicFormats();

    if (auto *reader = formatManager.createReaderFor(file)) {
        DBG("Playing file " << file.getFullPathName());
        auto source{std::make_unique<juce::AudioFormatReaderSource>(reader, true)};
        source->setLooping(true);
        transport.setSource(source.get());
        readerSource = std::move(source);
        transport.start();
    } else {
        jassertfalse;
    }
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(const int samplesPerBlockExpected, const double sampleRate)
{
    DBG("Device: [" << deviceManager.getCurrentAudioDeviceType() << "]" <<
        deviceManager.getCurrentAudioDevice()->getName() <<
        ", Sample rate: " << sampleRate <<
        ", Buffer size: " << samplesPerBlockExpected);

    transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
    server.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::releaseResources()
{
    transport.releaseResources();
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    if (readerSource != nullptr) {
        transport.getNextAudioBlock(bufferToFill);
    } else {
        bufferToFill.clearActiveBufferRegion();
    }

    // bufferToFill.buffer->clear(1, 0, bufferToFill.buffer->getNumSamples());
    server.getNextAudioBlock(bufferToFill);
}

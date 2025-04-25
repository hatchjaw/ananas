#include "MainComponent.h"

#include <utility>

MainComponent::MainComponent(const juce::File &file)
{
    // Apply desired audio settings.
    setAudioChannels(0, 2);
    auto setup{deviceManager.getAudioDeviceSetup()};
    setup.sampleRate = kSampleRate;
    setup.bufferSize = kNumFrames;
    deviceManager.setAudioDeviceSetup(setup, true);

    // Load the provided audio file.
    formatManager.registerBasicFormats();
    if (auto *reader = formatManager.createReaderFor(file)) {
        DBG("Loading file " << file.getFullPathName());
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        readerSource->setLooping(true);
        transport.setSource(readerSource.get());
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

    // Don't prepare to play until audio device setup has been updated.
    if (samplesPerBlockExpected == kNumFrames) {
        transport.prepareToPlay(samplesPerBlockExpected, sampleRate);
        server.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}

void MainComponent::releaseResources()
{
    formatManager.clearFormats();
    if (readerSource) readerSource->releaseResources();
    transport.setSource(nullptr);
    transport.releaseResources();
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)
{
    if (readerSource != nullptr) {
        // Write the latest audio file samples to the buffer.
        transport.getNextAudioBlock(bufferToFill);
    } else {
        bufferToFill.clearActiveBufferRegion();
    }

    // Not strictly a buffer to fill, as far as the server is concerned;
    // nonetheless, pass it to the server to be written to the outgoing queue.
    server.getNextAudioBlock(bufferToFill);
}

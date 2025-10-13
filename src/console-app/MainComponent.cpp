#include "MainComponent.h"

MainComponent::MainComponent(const juce::File &file) : server(2)
{
    // for (auto type: deviceManager.getAvailableDeviceTypes()) {
    //     for (auto name: type->getDeviceNames()) {
    //         DBG(name);
    //     }
    // }

    // Apply desired audio settings.
    setAudioChannels(0, 2);
    auto setup{deviceManager.getAudioDeviceSetup()};
    setup.sampleRate = kSampleRate;
    setup.bufferSize = kNumFrames;

    setup.outputDeviceName = "Teensy Ananas, USB Audio; Front output / input";

    std::cerr << deviceManager.setAudioDeviceSetup(setup, true) << std::endl;

    // Load the provided audio file.
    formatManager.registerBasicFormats();
    if (auto *reader = formatManager.createReaderFor(file)) {
        std::cout << "Loading file " << file.getFullPathName() << std::endl;
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
    std::cout << "\033[0;36m" <<
            "Device: [" << deviceManager.getCurrentAudioDeviceType() << "] " <<
            deviceManager.getCurrentAudioDevice()->getName() <<
            ", Sample rate: " << sampleRate <<
            ", Buffer size: " << samplesPerBlockExpected <<
            "\033[0m" << std::endl;

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

    bufferToFill.buffer->applyGain(.6f);

    // Not strictly a buffer to fill, as far as the server is concerned;
    // nonetheless, pass it to the server to be written to the outgoing queue.
    server.getNextAudioBlock(bufferToFill);
}

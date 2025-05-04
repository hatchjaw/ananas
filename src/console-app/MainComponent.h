#ifndef MAINCOMPONENT_H
#define MAINCOMPONENT_H

#include <juce_audio_utils/juce_audio_utils.h>
#include <AnanasServer.h>

class MainComponent final : public juce::AudioAppComponent {
public:
    explicit MainComponent(const juce::File &file);

    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    void releaseResources() override;

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

private:
    static constexpr int kNumFrames{128};
    static constexpr double kSampleRate{AUDIO_SAMPLE_RATE};

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transport;
    AnanasServer server;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};



#endif //MAINCOMPONENT_H

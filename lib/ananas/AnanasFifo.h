#ifndef ANANASFIFO_H
#define ANANASFIFO_H

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_events/juce_events.h>

using FormatConverter = juce::AudioData::ConverterInstance<
    juce::AudioData::Pointer<
        juce::AudioData::Float32,
        juce::AudioData::NativeEndian,
        juce::AudioData::NonInterleaved,
        juce::AudioData::Const
    >,
    juce::AudioData::Pointer<
        juce::AudioData::Int16,
        juce::AudioData::LittleEndian,
        juce::AudioData::Interleaved,
        juce::AudioData::NonConst
    >
>;

class AnanasFifo final : juce::Timer
{
public:
    AnanasFifo();

    bool isReady(int framesRequested) const;

    void write(const juce::AudioBuffer<float> *src);

    void read(uint8_t *dest, int numSamples);

    void timerCallback() override;

private:
    static constexpr int capacity{(1 << 10)};
    juce::AbstractFifo fifo{capacity};
    juce::AudioBuffer<float> buffer{2, capacity};
    FormatConverter converter{2, 2};
    std::mutex mutex;
    std::condition_variable condition;
};


#endif //ANANASFIFO_H

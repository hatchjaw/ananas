#ifndef ANANASFIFO_H
#define ANANASFIFO_H

#include <Utils.h>
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

namespace ananas
{
    class Fifo final : juce::Timer
    {
    public:
        Fifo(uint8_t numChannels);

        /**
         * Used as a condition_variable predicate.
         * @param framesRequested
         * @return
         */
        bool isReady(int framesRequested) const;

        /**
         * Write some samples to the FIFO. Called by the audio thread.
         * @param src
         */
        void write(const juce::AudioBuffer<float> *src);

        /**
         * Read some samples from the FIFO. Called by the network send thread.
         * @param dest
         * @param numSamples
         */
        void read(uint8_t *dest, int numSamples);

        void timerCallback() override;

        void abortRead();

    private:
        juce::AbstractFifo fifo{Constants::FifoCapacityFrames};
        std::unique_ptr<juce::AudioBuffer<float>> buffer;
        std::unique_ptr<FormatConverter> converter;
        std::mutex mutex;
        std::condition_variable condition;
        std::atomic<bool> shouldStop{false};
    };
}


#endif //ANANASFIFO_H

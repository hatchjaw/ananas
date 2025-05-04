#ifndef ANANASPACKET_H
#define ANANASPACKET_H
#include <juce_core/juce_core.h>


class AnanasPacket : public juce::MemoryBlock
{
public:
    struct Header
    {
        int64_t timestamp;
    };

    void prepare(int samplesPerBlockExpected, double sampleRate);

    uint8_t *getAudioData();

    void writeHeader();

private:
    static constexpr int64_t kNanoSecondsPerSecond{1'000'000'000};
    static constexpr int64_t kNanoSecondsPerPacket{kNanoSecondsPerSecond * 128 / AUDIO_SAMPLE_RATE};
    static constexpr double kNanoSecondsPerPacketRemainder{static_cast<double>(kNanoSecondsPerSecond) * 128 / AUDIO_SAMPLE_RATE - kNanoSecondsPerPacket};

    Header header{};
    double timestampRemainder{0};
    bool firstPacket{true};
};


#endif //ANANASPACKET_H

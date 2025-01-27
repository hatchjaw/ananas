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
    static constexpr int64_t nanoSecondsPerSecond{1'000'000'000};
    Header header{};
};


#endif //ANANASPACKET_H

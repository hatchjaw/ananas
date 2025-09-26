#ifndef ANANASPACKET_H
#define ANANASPACKET_H

#include <juce_core/juce_core.h>

namespace ananas
{
    class Packet : public juce::MemoryBlock
    {
    public:
        struct Header
        {
            int64_t timestamp;
        };

        void prepare(int samplesPerBlockExpected, double sampleRate);

        uint8_t *getAudioData();

        void writeHeader();

        void setTime(timespec ts);

        uint64_t getTime() const;

    private:
        static constexpr int64_t kNSPS{1'000'000'000};

        Header header{};
        int64_t nsPerPacket{};
        double nsPerPacketRemainder{};
        double timestampRemainder{0};
        double halfClientBufferDuration{};
    };
}

#endif //ANANASPACKET_H

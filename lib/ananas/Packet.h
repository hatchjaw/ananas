#ifndef ANANASPACKET_H
#define ANANASPACKET_H

#include <juce_core/juce_core.h>

namespace ananas
{
    class Packet : public juce::MemoryBlock
    {
    public:
        struct alignas(16) Header
        {
            int64_t timestamp;
            uint8_t numChannels;
            uint16_t numFrames;
        };

        void prepare(int numChannels, int framesPerPacket, double sampleRate);

        uint8_t *getAudioData();

        void writeHeader();

        void setTime(timespec ts);

        int64_t getTime() const;

        __syscall_slong_t getSleepInterval() const;

    private:
        Header header{};
        int64_t nsPerPacket{};
        __syscall_slong_t nsSleepInterval{};
        double nsPerPacketRemainder{};
        double timestampRemainder{0};
        double clientBufferDuration{};
    };
}

#endif //ANANASPACKET_H

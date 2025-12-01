#ifndef ANANASPACKET_H
#define ANANASPACKET_H

#include <juce_core/juce_core.h>

namespace ananas
{
    class AudioPacket : public juce::MemoryBlock
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

        [[nodiscard]] int64_t getTime() const;

        [[nodiscard]] __syscall_slong_t getSleepInterval() const;

    private:
        Header header{};
        int64_t nsPerPacket{};
        __syscall_slong_t nsSleepInterval{};
        double nsPerPacketRemainder{};
        double timestampRemainder{0};
        double clientBufferDuration{};
    };

    struct ClientAnnouncePacket
    {
        juce::uint32 serial;
        float samplingRate;
        float percentCPU;
        juce::int32 presentationOffsetFrame;
        juce::int64 presentationOffsetNs;
        juce::int32 audioPTPOffsetNs;
        juce::uint8 bufferFillPercent;
        bool ptpLock;
    };

    struct AuthorityAnnouncePacket
    {
        juce::uint32 serial;
        juce::uint32 usbFeedbackAccumulator;
        int numClients;
        int avgBufferFillPercent;
        int numUnderruns;
        int numOverflows;
    };
}

#endif //ANANASPACKET_H

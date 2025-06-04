#ifndef ANANASSERVER_H
#define ANANASSERVER_H

#include <juce_core/juce_core.h>
#include "AnanasFifo.h"
#include "AnanasPacket.h"

class AnanasServer final : public juce::AudioSource
{
public:
    AnanasServer();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    void releaseResources() override;

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

private:
    class Sender final : public juce::Thread
    {
    public:
        explicit Sender(AnanasFifo &fifo);

        void prepare(int samplesPerBlockExpected, double sampleRate);

        void run() override;

        void setPacketTime(timespec ts);

    private:
        juce::DatagramSocket socket;
        AnanasFifo &fifo;
        AnanasPacket packet{};
        int audioBlockSamples{0};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sender);
    };

    /**
     * A thread to listen out for a PTP timestamp; the first timestamp found
     * will dictate the timing reference for the Sender thread.
     */
    class TimestampListener final : public juce::Thread
    {
    public:
        TimestampListener();

        void prepare();

        void run() override;

        std::function<void(timespec)> onTimestamp;

    private:
        juce::DatagramSocket socket;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimestampListener);
    };

    AnanasFifo fifo;
    Sender sender;
    TimestampListener timestampListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnanasServer)
};


#endif //ANANASSERVER_H

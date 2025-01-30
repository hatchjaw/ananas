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

    private:
        juce::DatagramSocket socket;
        AnanasFifo &fifo;
        AnanasPacket packet{};
        int audioBlockSamples{0};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sender);
    };

    AnanasFifo fifo;
    Sender sender;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnanasServer)
};


#endif //ANANASSERVER_H

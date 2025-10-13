#ifndef ANANASSERVER_H
#define ANANASSERVER_H

#include <juce_core/juce_core.h>
#include "Fifo.h"
#include "Packet.h"
#include "ClientInfo.h"

namespace ananas
{
    class Server final : public juce::AudioSource
    {
    public:
        explicit Server(uint8_t numChannelsToSend);

        ~Server() override;

        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

        void releaseResources() override;

        void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

        ClientList *getClientList();

    private:
        class Sender final : public juce::Thread
        {
        public:
            explicit Sender(Fifo &fifo);

            bool prepare(int numChannels, int samplesPerBlockExpected, double sampleRate);

            void run() override;

            void setPacketTime(timespec ts);

            int64_t getPacketTime() const;

            bool stopThread(int timeOutMilliseconds);

        private:
            juce::DatagramSocket socket;
            Fifo &fifo;
            AudioPacket packet{};
            int audioBlockSamples{0};
            bool isReady{false};

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sender);
        };

        /**
         * A thread to listen out for PTP timestamps.
         */
        class TimestampListener final : public juce::Thread
        {
        public:
            TimestampListener();

            bool prepare();

            void run() override;

            std::function<void(timespec)> onTimestamp;

        private:
            juce::DatagramSocket socket;
            bool isReady{false};

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimestampListener);
        };

        class ClientListener final : public juce::Thread
        {
        public:
            explicit ClientListener(ClientList &clients);

            bool prepare();

            void run() override;

        private:
            juce::DatagramSocket socket;
            bool isReady{false};
            ClientList &clients;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClientListener);
        };

        uint8_t numChannels;
        Fifo fifo;
        Sender sender;
        TimestampListener timestampListener;
        ClientListener clientListener;
        ClientList clients;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Server)
    };
}


#endif //ANANASSERVER_H

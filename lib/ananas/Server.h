#ifndef ANANASSERVER_H
#define ANANASSERVER_H

#include <AuthorityInfo.h>
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

        AuthorityInfo *getAuthority();

    private:
        class Sender final : public juce::Thread
        {
        public:
            explicit Sender(Fifo &fifo);

            ~Sender() override;

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

        class AnnouncementListenerThread : public juce::Thread
        {
        public:
            AnnouncementListenerThread(const juce::String &threadName, const juce::String &multicastIP, int portToListenOn);

            ~AnnouncementListenerThread() override;

            bool prepare();

            void run() override = 0;

        protected:
            juce::DatagramSocket socket;
            bool isReady{false};
            juce::String ip;
            int port;
        };

        /**
         * A thread to listen out for PTP timestamps.
         */
        class TimestampListener final : public AnnouncementListenerThread
        {
        public:
            TimestampListener();

            void run() override;

            std::function<void(timespec)> onTimestamp;

        private:
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimestampListener);
        };

        class ClientListener final : public AnnouncementListenerThread
        {
        public:
            explicit ClientListener(ClientList &clients);

            void run() override;

        private:
            ClientList &clients;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClientListener);
        };

        class AuthorityListener final : public AnnouncementListenerThread
        {
        public:
            explicit AuthorityListener(AuthorityInfo &authority);

            void run() override;

            AuthorityInfo &authority;

        private:
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AuthorityListener);
        };

        uint8_t numChannels;
        Fifo fifo;
        Sender sender;
        TimestampListener timestampListener;
        ClientListener clientListener;
        ClientList clients;
        AuthorityListener authorityListener;
        AuthorityInfo authority;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Server)
    };
}


#endif //ANANASSERVER_H

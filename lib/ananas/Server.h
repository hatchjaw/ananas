#ifndef ANANASSERVER_H
#define ANANASSERVER_H

#include <juce_core/juce_core.h>
#include "ClientInfo.h"
#include "AuthorityInfo.h"
#include "SwitchInfo.h"
#include "Fifo.h"
#include "Packet.h"

namespace ananas
{
    class Server final : public juce::AudioSource,
                         public juce::ChangeListener,
                         public juce::ChangeBroadcaster
    {
    public:
        explicit Server(uint8_t numChannelsToSend);

        ~Server() override;

        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

        void releaseResources() override;

        void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

        void changeListenerCallback(ChangeBroadcaster *source) override;

        [[nodiscard]] bool isConnected() const;

        ClientList *getClientList();

        ModuleList *getModuleList();

        AuthorityInfo *getAuthority();

        SwitchList *getSwitches();

    private:
        //======================================================================

        class AnanasThread : public juce::Thread,
                             public ChangeBroadcaster
        {
        public:
            AnanasThread(const juce::String &threadName, int timeoutMs);

            virtual bool connect() = 0;

            void run() override;

            int getTimeout() const;

            bool isConnected() const;

        protected:
            virtual void runImpl() = 0;

            int timeoutMs{0};
            bool connected{false};
        };

        //======================================================================

        class UDPMulticastThread : public AnanasThread
        {
        public:
            UDPMulticastThread(const juce::String &threadName, juce::String multicastIP, int timeoutMs);

            ~UDPMulticastThread() override;

            bool connect() override;

        protected:
            void runImpl() override = 0;

            juce::DatagramSocket socket;
            juce::String ip;
        };

        //======================================================================

        class AudioSender final : public UDPMulticastThread,
                                  public ChangeListener
        {
        public:
            explicit AudioSender(Fifo &fifo);

            bool prepare(int numChannels, int samplesPerBlockExpected, double sampleRate);

            void setPacketTime(timespec ts);

            int64_t getPacketTime() const;

            bool stopThread(int timeOutMilliseconds);

            void changeListenerCallback(ChangeBroadcaster *source) override;

        protected:
            void runImpl() override;

        private:
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSender);

            Fifo &fifo;
            AudioPacket packet{};
            int audioBlockSamples{0};
        };

        //======================================================================

        class AnnouncementListenerThread : public UDPMulticastThread
        {
        public:
            AnnouncementListenerThread(const juce::String &threadName, const juce::String &multicastIP, int timeoutMs, int portToListenOn);

            bool connect() override;

        protected:
            void runImpl() override = 0;

            int port;
        };

        //======================================================================

        /**
         * A thread to listen out for PTP timestamps.
         */
        class TimestampListener final : public AnnouncementListenerThread
        {
        public:
            TimestampListener();

            timespec getPacketTime() const;

            bool getTimestampChanged() const;

        protected:
            void runImpl() override;

        private:
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimestampListener);
            timespec timestamp{};
            bool timestampChanged{false};
        };

        //======================================================================

        class ClientListener final : public AnnouncementListenerThread
        {
        public:
            ClientListener(ClientList &clients, ModuleList &modules);

        protected:
            void runImpl() override;

        private:
            ClientList &clients;
            ModuleList &modules;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClientListener);
        };

        //======================================================================

        class AuthorityListener final : public AnnouncementListenerThread
        {
        public:
            explicit AuthorityListener(AuthorityInfo &authority);

        protected:
            void runImpl() override;

        private:
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AuthorityListener);

            AuthorityInfo &authority;
        };

        //======================================================================

        class RebootSender final : public UDPMulticastThread
        {
        public:
            explicit RebootSender(ClientList &clients);

            bool prepare();

        protected:
            void runImpl() override;

        private:
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RebootSender)

            ClientList &clients;
        };

        //======================================================================

        class SwitchInspector final : public AnanasThread
        {
        public:
            explicit SwitchInspector(SwitchList &switches);

            void runImpl() override;

            bool connect() override;

        private:
            SwitchList &switches;
            juce::ChildProcess curl;
            int curlTimeoutS{0};

            /**
             * E.g. curl -s -k -u admin:emeraude http://192.168.10.1/rest/system/ptp/monitor -d \'{"numbers":"0","once":""}\' -H "content-type: application/json"
             * @param ip
             * @param username
             * @param password
             * @param path
             * @param postData
             * @return
             */
            juce::var curlRequest(const juce::String &ip,
                                  const juce::String &username,
                                  const juce::String &password,
                                  const juce::StringRef &path,
                                  const juce::String &postData);

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwitchInspector);
        };

        //======================================================================

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Server)

        uint8_t numChannels;
        Fifo fifo;
        SwitchList switches;
        ClientList clients;
        ModuleList modules;
        AuthorityInfo authority;
        juce::OwnedArray<AnanasThread> threads;
    };
}


#endif //ANANASSERVER_H

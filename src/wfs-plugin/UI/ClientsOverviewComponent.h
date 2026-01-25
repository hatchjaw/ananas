#ifndef CLIENTSOVERVIEWCOMPONENT_H
#define CLIENTSOVERVIEWCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "AnanasNetworkTable.h"
#include "../Utils.h"

namespace ananas
{
    class ClientsOverviewComponent final : public juce::Component,
                                           public juce::ValueTree::Listener,
                                           juce::AsyncUpdater
    {
    public:
        explicit ClientsOverviewComponent(juce::ValueTree &treeToListenTo);

        ~ClientsOverviewComponent() override;

        void update(const juce::var &var);

        void paint(juce::Graphics &g) override;

        void resized() override;

        void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

        void handleAsyncUpdate() override;

        void triggerClientReboot() const;

    private:
        class OverviewPanel final : public Component
        {
        public:
            OverviewPanel();

            void update(const juce::var &var);

            void paint(juce::Graphics &g) override;

            void resized() override;

        private:
            juce::Label totalClientsLabel;
            juce::Label totalClientsValue;
            juce::Label presentationTimeIntervalLabel;
            juce::Label presentationTimeIntervalValue;
        };

        class ClientTable final : public AnanasNetworkTable
        {
        public:
            ClientTable();

            ~ClientTable() override;

            void update(const juce::var &clientInfo);

            int getNumRows() override;

            void paintRowBackground(juce::Graphics &g, int rowNumber, int width,
                                    int height, bool rowIsSelected) override;

            void paintCell(juce::Graphics &g, int rowNumber, int columnId,
                           int width, int height, bool rowIsSelected) override;

            void resized() override;

        private:
            class LookAndFeel final : public juce::LookAndFeel_V4
            {
            public:
                void drawTableHeaderColumn(juce::Graphics &, juce::TableHeaderComponent &,
                                           const juce::String &columnName, int columnId, int width, int height,
                                           bool isMouseOver, bool isMouseDown, int columnFlags) override;
            };

            struct Row
            {
                juce::String ip;
                juce::String serialNumber;
                bool ptpLock;
                juce::int32 presentationTimeOffsetNs;
                juce::int32 presentationTimeOffsetFrame;
                juce::int32 audioPTPOffsetNs;
                juce::int32 bufferFillPercent;
                float samplingRate;
                float percentCPU;
                juce::int32 moduleID;
            };

            juce::Array<Row> rows;
            LookAndFeel lookAndFeel;
        };

        juce::Label title;
        juce::TextButton rebootAllClientsButton;
        OverviewPanel overviewPanel;
        ClientTable clientTable;
        juce::ValueTree &tree;
    };
}


#endif //CLIENTSOVERVIEWCOMPONENT_H

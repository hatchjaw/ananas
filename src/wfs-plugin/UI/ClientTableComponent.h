#ifndef CLIENTTABLECOMPONENT_H
#define CLIENTTABLECOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>

#include "../Utils.h"

namespace ananas
{
    class ClientTableComponent final : public juce::Component,
                                       public juce::TableListBoxModel
    {
    public:
        ClientTableComponent();

        ~ClientTableComponent() override;

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
        };

        void addColumn(const WFS::TableColumns::ColumnHeader &h) const;

        juce::TableListBox table{{}, this};
        juce::Array<Row> rows;
        LookAndFeel lookAndFeel;
    };
} // ananas

#endif //CLIENTTABLECOMPONENT_H

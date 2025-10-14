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

        void update(const juce::var &clientInfo);

        int getNumRows() override;

        void paintRowBackground(juce::Graphics &g, int rowNumber, int width,
                                int height, bool rowIsSelected) override;

        void paintCell(juce::Graphics &g, int rowNumber, int columnId,
                       int width, int height, bool rowIsSelected) override;

        void resized() override;

    private:
        void addColumn(const WFS::Tables::ColumnHeader &h) const;
        struct Row
        {
            juce::String ip;
            juce::String serialNumber;
            juce::int32 offsetTime;
            juce::int32 offsetFrame;
            juce::int32 bufferFillPercent;
            float samplingRate;
            float percentCPU;
        };
        juce::TableListBox table{{}, this};
        juce::Array<Row> rows;
    };
} // ananas

#endif //CLIENTTABLECOMPONENT_H

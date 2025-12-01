#ifndef TIMEAUTHORITYCOMPONENT_H
#define TIMEAUTHORITYCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utils.h"

namespace ananas
{
    class TimeAuthorityComponent final : public juce::Component
    {
    public:
        TimeAuthorityComponent();

        void update(juce::var var);

        void paint(juce::Graphics &g) override;

        void resized() override;

    private:
        class TimeAuthorityTable final : public Component,
                                         public juce::TableListBoxModel
        {
        public:
            TimeAuthorityTable();

            void update(const juce::var &var);

            int getNumRows() override;

            void paintRowBackground(juce::Graphics &, int rowNumber, int width, int height, bool rowIsSelected) override;

            void paintCell(juce::Graphics &, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

            void resized() override;

        private:
            struct Row
            {
                juce::String ip;
                juce::String serialNumber;
                juce::String feedbackAccumulator;
                juce::String samplingRate;
            };

            void addColumn(const WFS::TableColumns::ColumnHeader &h) const;

            Row row{};
            juce::TableListBox table{{}, this};
        };

        juce::Label title;
        TimeAuthorityTable authorityTable;
    };
}


#endif //TIMEAUTHORITYCOMPONENT_H

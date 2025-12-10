#ifndef TIMEAUTHORITYCOMPONENT_H
#define TIMEAUTHORITYCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utils.h"

namespace ananas
{
    class TimeAuthorityComponent final : public juce::Component,
                                         public juce::ValueTree::Listener,
                                         juce::AsyncUpdater

    {
    public:
        explicit TimeAuthorityComponent(juce::ValueTree &treeToListenTo);

        ~TimeAuthorityComponent() override;

        void update(juce::var var);

        void paint(juce::Graphics &g) override;

        void resized() override;

        void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

        void handleAsyncUpdate() override;

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
        juce::ValueTree &tree;
    };
}


#endif //TIMEAUTHORITYCOMPONENT_H

#ifndef SWITCHESCOMPONENT_H
#define SWITCHESCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Utils.h"

namespace ananas
{
    class SwitchesComponent final : public juce::Component,
                                    public juce::ValueTree::Listener,
                                    juce::AsyncUpdater
    {
    public:
        SwitchesComponent(juce::ValueTree &dynamicTreeRef, juce::ValueTree &persistentTreeRef);

        ~SwitchesComponent() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

        void update(const juce::var &var);

        void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

        void handleAsyncUpdate() override;

        void removeSwitch(int switchID) const;

    private:
        class SwitchesTable final : public Component,
                                    public juce::TableListBoxModel
        {
        public:
            SwitchesTable();

            void update(const juce::var &var);

            int getNumRows() override;

            void paintRowBackground(juce::Graphics &, int rowNumber, int width, int height, bool rowIsSelected) override;

            void paintCell(juce::Graphics &, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

            void resized() override;

            Component *refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate) override;

            void addSwitch() const;

            void handleResetPtpForSwitch(int rowNumber);

            void handleRemoveSwitch(int rowNumber) const;

            std::function<void(int rowNumber, int columnId, juce::String value)> onCellEdited;
            std::function<void(int switchID)> onSwitchRemoved;
            bool isEditing{false};

        private:
            struct Row
            {
                juce::String ip;
                juce::String username;
                juce::String password;
                juce::int32 freqDriftPPB{0};
                juce::int32 offsetNS{0};
            };

            void addColumn(const WFS::TableColumns::ColumnHeader &h) const;

            inline static const juce::Array<int> editableColumnIDs{1, 2, 3};
            juce::TableListBox table{{}, this};
            juce::HashMap<int, Row> rows;
        };

        void updateSwitch(int row, int col, const juce::String &content) const;

        juce::Label title;
        juce::TextButton addSwitchButton;
        SwitchesTable switchesTable;
        juce::ValueTree &dynamicTree;
        juce::ValueTree &persistentTree;
    };
} // ananas

#endif //SWITCHESCOMPONENT_H

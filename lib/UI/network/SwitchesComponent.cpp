#include "SwitchesComponent.h"
#include <AnanasUtils.h>

namespace ananas::UI
{
    SwitchesComponent::SwitchesComponent(juce::ValueTree &dynamicTreeRef, juce::ValueTree &persistentTreeRef)
        : addSwitchButton(Strings::AddSwitchButtonName),
          dynamicTree(dynamicTreeRef),
          persistentTree(persistentTreeRef)
    {
        addAndMakeVisible(title);
        addAndMakeVisible(addSwitchButton);
        addAndMakeVisible(switchesTable);

        title.setColour(juce::Label::textColourId, juce::Colours::black);
        title.setFont(juce::Font(juce::FontOptions(15.f, juce::Font::bold)));
        title.setJustificationType(juce::Justification::centredLeft);
        title.setText(Strings::SwitchesSectionTitle, juce::dontSendNotification);

        addSwitchButton.setButtonText(Strings::AddSwitchButtonText);
        addSwitchButton.setTooltip(Strings::AddSwitchButtonTooltip);
        addSwitchButton.onClick = [this]
        {
            addSwitch();
        };

        dynamicTree.addListener(this);
        persistentTree.addListener(this);

        switchesTable.onCellEdited = [this](const int row, const int col, const juce::String &content)
        {
            updateSwitch(switchesTable.getSwitchID(row), col, content);
        };

        switchesTable.onResetPtpForSwitch = [this](const juce::Identifier &switchID)
        {
            resetPtpForSwitch(switchID);
        };

        switchesTable.onSwitchRemoved = [this](const juce::Identifier &switchID)
        {
            removeSwitch(switchID);
        };
    }

    SwitchesComponent::~SwitchesComponent()
    {
        persistentTree.removeListener(this);
        dynamicTree.removeListener(this);
    }

    void SwitchesComponent::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    void SwitchesComponent::resized()
    {
        auto bounds{getLocalBounds()};
        auto titleRow{
            bounds.removeFromTop(Dimensions::NetworkSectionTitleHeight)
            .reduced(6, 0)
        };
        title.setBounds(titleRow.removeFromLeft(85));
        addSwitchButton.setBounds(titleRow.removeFromLeft(45).reduced(8));
        switchesTable.setBounds(bounds);
    }

    void SwitchesComponent::update(const juce::var &var)
    {
        switchesTable.update(var);
    }

    void SwitchesComponent::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
    {
        if (!isVisible()) return;

        if (property == ananas::Identifiers::SwitchesParamID) {
            update(treeWhosePropertyHasChanged[property]);
            handleAsyncUpdate();
        }
    }

    void SwitchesComponent::handleAsyncUpdate()
    {
        repaint();
    }

    void SwitchesComponent::addSwitch() const
    {
        juce::Random r;

        const auto switchID{juce::Identifier{ananas::Identifiers::SwitchIdentifierBase + juce::String{abs(r.nextInt())}}};

        updateSwitch(switchID, 0, "");
    }

    void SwitchesComponent::removeSwitch(const juce::Identifier &switchID) const
    {
        const auto switchesVar{persistentTree.getProperty(ananas::Identifiers::SwitchesParamID)};

        const auto switchesObject{switchesVar.getDynamicObject()};

        const auto switchVar = switchesObject->getProperty(switchID);
        switchVar.getDynamicObject()->setProperty(ananas::Identifiers::SwitchShouldRemovePropertyID, true);
        switchesObject->setProperty(switchID, switchVar);

        persistentTree.setProperty(ananas::Identifiers::SwitchesParamID, switchesVar, nullptr);
        persistentTree.sendPropertyChangeMessage(ananas::Identifiers::SwitchesParamID);
    }

    void SwitchesComponent::resetPtpForSwitch(const juce::Identifier &switchID) const
    {
        const auto switchesVar{dynamicTree.getProperty(ananas::Identifiers::SwitchesParamID)};

        const auto switchesObject{switchesVar.getDynamicObject()};

        const auto switchVar = switchesObject->getProperty(switchID);
        switchVar.getDynamicObject()->setProperty(ananas::Identifiers::SwitchShouldResetPtpPropertyID, true);
        switchesObject->setProperty(switchID, switchVar);

        dynamicTree.setProperty(ananas::Identifiers::SwitchesParamID, switchesVar, nullptr);
        dynamicTree.sendPropertyChangeMessage(ananas::Identifiers::SwitchesParamID);
    }

    void SwitchesComponent::updateSwitch(const juce::Identifier &switchID, const int col, const juce::String &content) const
    {
        auto switchesVar{persistentTree.getProperty(ananas::Identifiers::SwitchesParamID)};

        if (!switchesVar.isObject()) {
            switchesVar = new juce::DynamicObject;
        }

        const auto switchesObject{switchesVar.getDynamicObject()};

        auto rowVar = switchesObject->getProperty(switchID);
        if (!rowVar.isObject()) {
            rowVar = new juce::DynamicObject;
            rowVar.getDynamicObject()->setProperty(ananas::Identifiers::SwitchIpPropertyID, "");
            rowVar.getDynamicObject()->setProperty(ananas::Identifiers::SwitchUsernamePropertyID, "");
            rowVar.getDynamicObject()->setProperty(ananas::Identifiers::SwitchPasswordPropertyID, "");
            switchesObject->setProperty(switchID, rowVar);
        }

        const auto rowObject = rowVar.getDynamicObject();

        // Update the appropriate property based on column ID
        switch (col) {
            case 1: rowObject->setProperty(ananas::Identifiers::SwitchIpPropertyID, content);
                break;
            case 2: rowObject->setProperty(ananas::Identifiers::SwitchUsernamePropertyID, content);
                break;
            case 3: rowObject->setProperty(ananas::Identifiers::SwitchPasswordPropertyID, content);
                break;
            default: break;
        }

        // Write back to trigger change notification
        persistentTree.setProperty(ananas::Identifiers::SwitchesParamID, switchesVar, nullptr);
        persistentTree.sendPropertyChangeMessage(ananas::Identifiers::SwitchesParamID);
    }

    //==========================================================================

    SwitchesComponent::SwitchesTable::SwitchesTable()
    {
        addAndMakeVisible(table);

        addColumn(TableColumns::SwitchesTableIpAddress);
        addColumn(TableColumns::SwitchesTableUsername);
        addColumn(TableColumns::SwitchesTablePassword);
        addColumn(TableColumns::SwitchesTableDrift);
        addColumn(TableColumns::SwitchesTableOffset);
        addColumn(TableColumns::SwitchesTableResetPTP);
        addColumn(TableColumns::SwitchesTableRemoveSwitch);

        table.setModel(this);
        table.setColour(juce::ListBox::outlineColourId, juce::Colours::black);
        table.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentWhite);
        table.setOutlineThickness(1);
    }

    void SwitchesComponent::SwitchesTable::update(const juce::var &var)
    {
        if (!isVisible() || isEditing) return;

        rows.clear();

        if (auto *obj = var.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                Row row;

                if (const auto *theSwitch = prop.value.getDynamicObject()) {
                    row.id = prop.name;
                    row.ip = theSwitch->getProperty(ananas::Identifiers::SwitchIpPropertyID);
                    row.username = theSwitch->getProperty(ananas::Identifiers::SwitchUsernamePropertyID);
                    row.password = theSwitch->getProperty(ananas::Identifiers::SwitchPasswordPropertyID);
                    row.freqDriftPPB = theSwitch->getProperty(ananas::Identifiers::SwitchFreqDriftPropertyId);
                    row.offsetNS = theSwitch->getProperty(ananas::Identifiers::SwitchOffsetPropertyId);
                }

                rows.add(row);
            }
        }

        table.updateContent();
        repaint();
    }

    int SwitchesComponent::SwitchesTable::getNumRows()
    {
        return rows.size();
    }

    void SwitchesComponent::SwitchesTable::paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
    {
        juce::ignoreUnused(width, height);

        if (rowIsSelected)
            g.fillAll(juce::Colours::lightblue);
        else if (rowNumber % 2 == 0)
            g.fillAll(juce::Colour(0xffeeeeee));
        else
            g.fillAll(juce::Colours::white);
    }

    void SwitchesComponent::SwitchesTable::paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
    {
        juce::ignoreUnused(rowIsSelected);

        if (rowNumber < rows.size()) {
            const auto &[id, ip, username, password, drift, offset] = rows[rowNumber];
            juce::String text;
            juce::Justification justification{juce::Justification::centredLeft};

            switch (columnId) {
                case 4: text = Strings::formatWithThousandsSeparator(drift);
                    justification = TableColumns::SwitchesTableDrift.justification;
                    break;
                case 5: text = Strings::formatWithThousandsSeparator(offset);
                    justification = TableColumns::SwitchesTableOffset.justification;
                    break;
                default: break;
            }

            g.setColour(juce::Colours::black);
            g.setFont(14.0f);
            g.drawText(text, 2, 0, width - 4, height, justification, true);
        }
    }

    void SwitchesComponent::SwitchesTable::resized()
    {
        table.setBounds(getLocalBounds().reduced(10));
    }

    juce::Component *SwitchesComponent::SwitchesTable::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                                                               Component *existingComponentToUpdate)
    {
        juce::ignoreUnused(isRowSelected);

        if (editableColumnIDs.contains(columnId)) {
            auto *textEditor = dynamic_cast<juce::TextEditor *>(existingComponentToUpdate);

            if (textEditor == nullptr) {
                textEditor = new juce::TextEditor{"Switch editor row " + juce::String{rowNumber} + " col " + juce::String{columnId}};
                textEditor->setMultiLine(false);
                textEditor->setReturnKeyStartsNewLine(false);
                textEditor->setWantsKeyboardFocus(true);
                textEditor->setMouseClickGrabsKeyboardFocus(true);
                textEditor->setExplicitFocusOrder(1);

                if (columnId == passwordColumnID) {
                    textEditor->setPasswordCharacter(L'•');
                }

                textEditor->onTextChange = [this]()
                {
                    isEditing = true;
                };

                textEditor->onFocusLost = [this, rowNumber, columnId]
                {
                    const auto *editor = dynamic_cast<juce::TextEditor *>(
                        table.getCellComponent(columnId, rowNumber)
                    );
                    if (editor != nullptr && onCellEdited) {
                        switch (columnId) {
                            case 1: rows[rowNumber].ip = editor->getText();
                                break;
                            case 2: rows[rowNumber].username = editor->getText();
                                break;
                            case 3: rows[rowNumber].password = editor->getText();
                                break;
                            default: break;
                        }
                        onCellEdited(rowNumber, columnId, editor->getText());
                    }
                    isEditing = false;
                };

                textEditor->onReturnKey = [textEditor]()
                {
                    textEditor->moveKeyboardFocusToSibling(true);
                };
            }

            switch (columnId) {
                case 1: textEditor->setText(rows[rowNumber].ip, juce::dontSendNotification);
                    break;
                case 2: textEditor->setText(rows[rowNumber].username, juce::dontSendNotification);
                    break;
                case 3: textEditor->setText(rows[rowNumber].password, juce::dontSendNotification);
                    break;
                default: break;
            }

            return textEditor;
        } else if (columnId == 6) {
            auto *button{dynamic_cast<juce::Button *>(existingComponentToUpdate)};

            if (button == nullptr) {
                button = new juce::TextButton{Strings::ResetSwitchButtonText};
                button->onClick = [this, rowNumber]
                {
                    handleResetPtpForSwitch(rows[rowNumber].id);
                };
            }

            return button;
        } else if (columnId == 7) {
            auto *button{dynamic_cast<juce::Button *>(existingComponentToUpdate)};

            if (button == nullptr) {
                button = new juce::TextButton{Strings::RemoveSwitchButtonText};
                button->onClick = [this, rowNumber]
                {
                    handleRemoveSwitch(rows[rowNumber].id);
                };
            }

            return button;
        }

        return nullptr;
    }

    void SwitchesComponent::SwitchesTable::handleResetPtpForSwitch(const juce::Identifier &switchID) const
    {
        if (onResetPtpForSwitch)
            onResetPtpForSwitch(switchID);
    }

    void SwitchesComponent::SwitchesTable::handleRemoveSwitch(const juce::Identifier &switchID) const
    {
        if (onSwitchRemoved)
            onSwitchRemoved(switchID);
    }

    juce::Identifier SwitchesComponent::SwitchesTable::getSwitchID(const int rowNumber) const
    {
        return rows[rowNumber].id;
    }
} // ananas::UI

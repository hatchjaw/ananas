#include "ModuleSelectorComponent.h"
#include <AnanasUtils.h>
#include "../../Utils.h"

namespace ananas::WFS
{
    ModuleSelectorComponent::ModuleSelectorComponent(const uint moduleIndex, juce::ValueTree &persistentTree)
        : tree(persistentTree),
          index(moduleIndex)
    {
        addAndMakeVisible(comboBox);
        addAndMakeVisible(speakerIcon1);
        addAndMakeVisible(speakerIcon2);

        comboBox.onChange = [this]
        {
            setIndexForModule();
        };
    }

    void ModuleSelectorComponent::resized()
    {
        auto bounds{getLocalBounds()};
        const auto comboBoxBounds{bounds.removeFromTop(Constants::UI::ModuleSelectorComboBoxHeight + 1)};
        comboBox.setBounds(comboBoxBounds);

        juce::FlexBox flex;
        flex.flexDirection = juce::FlexBox::Direction::row;
        flex.justifyContent = juce::FlexBox::JustifyContent::center;
        flex.items.add(juce::FlexItem(speakerIcon1)
            .withFlex(1.f)
            .withHeight(Constants::UI::ModuleSelectorSpeakerHeight));
        flex.items.add(juce::FlexItem(speakerIcon2)
            .withFlex(1.f)
            .withHeight(Constants::UI::ModuleSelectorSpeakerHeight));

        flex.performLayout(bounds);
    }

    void ModuleSelectorComponent::setAvailableModules(const juce::StringArray &ips)
    {
        comboBox.clear(juce::dontSendNotification);
        comboBox.addItemList(ips, 1);

        auto modules{tree.getProperty(ananas::Identifiers::ModulesParamID)};
        auto varIntN{juce::var{static_cast<int>(index)}};
        if (auto *obj = modules.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                if (prop.value.getProperty(ananas::Identifiers::ModuleIDPropertyID, 0) == varIntN) {
                    setSelectedModule(prop.name.toString());
                }
            }
        }
    }

    void ModuleSelectorComponent::setIndexForModule() const
    {
        auto modules{tree.getProperty(ananas::Identifiers::ModulesParamID)};
        if (auto *obj = modules.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                if (prop.name.toString() == comboBox.getText()) {
                    auto module{obj->getProperty(prop.name).getDynamicObject()};
                    module->setProperty(ananas::Identifiers::ModuleIDPropertyID, static_cast<int>(index));
                    tree.sendPropertyChangeMessage(ananas::Identifiers::ModulesParamID);
                }
            }
        }
    }

    void ModuleSelectorComponent::setSelectedModule(const juce::var &var)
    {
        if (var.isString()) {
            comboBox.setText(var.toString(), juce::dontSendNotification);
        }
    }

    //==========================================================================

    void ModuleSelectorComponent::SpeakerIconComponent::paint(juce::Graphics &g)
    {
        const auto speaker = createSpeakerPath();

        const auto targetBounds = juce::Rectangle(0.f, 0.f,
                                                  getBounds().toFloat().getWidth(),
                                                  getBounds().toFloat().getHeight() - Constants::UI::SpeakerIconOutlineThickness);

        const juce::RectanglePlacement placement{juce::RectanglePlacement::stretchToFit};
        const auto transform = placement.getTransformToFit(
            speaker.getBounds(), targetBounds);

        g.setColour(Constants::UI::SpeakerIconFillColour);
        g.fillPath(speaker, transform);

        g.setColour(Constants::UI::SpeakerIconOutlineColour);
        g.strokePath(speaker, juce::PathStrokeType{Constants::UI::SpeakerIconOutlineThickness, juce::PathStrokeType::mitered}, transform);
    }

    juce::Path ModuleSelectorComponent::SpeakerIconComponent::createSpeakerPath()
    {
        juce::Path speaker;

        speaker.startNewSubPath(Constants::UI::SpeakerIconCoilStartX, Constants::UI::SpeakerIconCoilStartY);
        speaker.lineTo(Constants::UI::SpeakerIconCoilStartX + Constants::UI::SpeakerIconCoilWidth, Constants::UI::SpeakerIconCoilStartY);
        speaker.lineTo(Constants::UI::SpeakerIconCoilStartX + Constants::UI::SpeakerIconCoilWidth, Constants::UI::SpeakerIconCoilHeight);
        speaker.lineTo(Constants::UI::SpeakerIconConeRightX, Constants::UI::SpeakerIconConeEndY);
        speaker.lineTo(Constants::UI::SpeakerIconConeLeftX, Constants::UI::SpeakerIconConeEndY);
        speaker.lineTo(Constants::UI::SpeakerIconCoilStartX, Constants::UI::SpeakerIconCoilHeight);
        speaker.closeSubPath();

        return speaker;
    }
} // ananas::WFS

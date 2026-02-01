#include "ModuleComponent.h"
#include <AnanasUtils.h>
#include "../../Utils.h"

namespace ananas::WFS
{
    ModuleComponent::ModuleComponent(const uint moduleIndex, juce::ValueTree &persistentTree)
        : tree(persistentTree),
          index(moduleIndex)
    {
        addAndMakeVisible(moduleSelector);
        addAndMakeVisible(speakerIcon1);
        addAndMakeVisible(speakerIcon2);

        moduleSelector.onChange = [this]
        {
            setIndexForModule();
        };
    }

    void ModuleComponent::resized()
    {
        auto bounds{getLocalBounds()};

        if (showModuleSelector) {
            const auto comboBoxBounds{bounds.removeFromTop(Constants::UI::ModuleSelectorHeight)};
            moduleSelector.setBounds(comboBoxBounds);
        } else {
            moduleSelector.setBounds(0, 0, 0, 0);
        }

        juce::FlexBox flex;
        flex.flexDirection = juce::FlexBox::Direction::row;
        flex.justifyContent = juce::FlexBox::JustifyContent::center;
        flex.items.add(juce::FlexItem(speakerIcon1)
            .withFlex(1.f)
            .withHeight(Constants::UI::ModuleSpeakerHeight));
        flex.items.add(juce::FlexItem(speakerIcon2)
            .withFlex(1.f)
            .withHeight(Constants::UI::ModuleSpeakerHeight));

        flex.performLayout(bounds);
    }

    void ModuleComponent::setAvailableModules(const juce::StringArray &ips)
    {
        moduleSelector.clear(juce::dontSendNotification);
        moduleSelector.addItemList(ips, 1);

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

    void ModuleComponent::setIndexForModule() const
    {
        auto modules{tree.getProperty(ananas::Identifiers::ModulesParamID)};
        if (auto *obj = modules.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                if (prop.name.toString() == moduleSelector.getText()) {
                    auto module{obj->getProperty(prop.name).getDynamicObject()};
                    module->setProperty(ananas::Identifiers::ModuleIDPropertyID, static_cast<int>(index));
                    tree.sendPropertyChangeMessage(ananas::Identifiers::ModulesParamID);
                }
            }
        }
    }

    void ModuleComponent::shouldShowModuleSelector(const bool show)
    {
        showModuleSelector = show;
        resized();
    }

    void ModuleComponent::setSelectedModule(const juce::var &var)
    {
        if (var.isString()) {
            moduleSelector.setText(var.toString(), juce::dontSendNotification);
        }
    }

    //==========================================================================

    void ModuleComponent::SpeakerIconComponent::paint(juce::Graphics &g)
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

    juce::Path ModuleComponent::SpeakerIconComponent::createSpeakerPath()
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

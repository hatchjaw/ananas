#include "ModuleComponent.h"
#include <AnanasUtils.h>
#include "../../WFSUtils.h"

namespace ananas::WFS::UI
{
    ModuleComponent::ModuleComponent(const int moduleIndex, juce::ValueTree &persistentTree)
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
            const auto comboBoxBounds{bounds.removeFromTop(Dimensions::ModuleSelectorHeight)};
            moduleSelector.setBounds(comboBoxBounds);
        } else {
            moduleSelector.setBounds(0, 0, 0, 0);
        }

        juce::FlexBox flex;
        flex.flexDirection = juce::FlexBox::Direction::row;
        flex.justifyContent = juce::FlexBox::JustifyContent::center;
        flex.items.add(juce::FlexItem(speakerIcon1)
            .withFlex(1.f)
            .withHeight(Dimensions::ModuleSpeakerHeight));
        flex.items.add(juce::FlexItem(speakerIcon2)
            .withFlex(1.f)
            .withHeight(Dimensions::ModuleSpeakerHeight));

        flex.performLayout(bounds);
    }

    void ModuleComponent::setAvailableModules(const juce::StringArray &ips)
    {
        moduleSelector.clear(juce::dontSendNotification);
        moduleSelector.addItemList(ips, 1);

        const auto modules{tree.getProperty(ananas::Utils::Identifiers::ModulesParamID)};
        const auto varIntN{juce::var{index}};
        if (auto *obj = modules.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                if (prop.value.getProperty(ananas::Utils::Identifiers::ModuleIDPropertyID, 0) == varIntN) {
                    setSelectedModule(prop.name.toString());
                }
            }
        }
    }

    void ModuleComponent::setIndexForModule() const
    {
        auto modules{tree.getProperty(ananas::Utils::Identifiers::ModulesParamID)};
        if (auto *obj = modules.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                if (prop.name.toString() == moduleSelector.getText()) {
                    auto module{obj->getProperty(prop.name).getDynamicObject()};
                    module->setProperty(ananas::Utils::Identifiers::ModuleIDPropertyID, static_cast<int>(index));
                    tree.sendPropertyChangeMessage(ananas::Utils::Identifiers::ModulesParamID);
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
                                                  getBounds().toFloat().getHeight() - Dimensions::SpeakerIconOutlineThickness);

        const juce::RectanglePlacement placement{juce::RectanglePlacement::stretchToFit};
        const auto transform = placement.getTransformToFit(
            speaker.getBounds(), targetBounds);

        g.setColour(Colours::SpeakerIconFillColour);
        g.fillPath(speaker, transform);

        g.setColour(Colours::SpeakerIconOutlineColour);
        g.strokePath(speaker, juce::PathStrokeType{Dimensions::SpeakerIconOutlineThickness, juce::PathStrokeType::mitered}, transform);
    }

    juce::Path ModuleComponent::SpeakerIconComponent::createSpeakerPath()
    {
        juce::Path speaker;

        speaker.startNewSubPath(Dimensions::SpeakerIconCoilStartX, Dimensions::SpeakerIconCoilStartY);
        speaker.lineTo(Dimensions::SpeakerIconCoilStartX + Dimensions::SpeakerIconCoilWidth, Dimensions::SpeakerIconCoilStartY);
        speaker.lineTo(Dimensions::SpeakerIconCoilStartX + Dimensions::SpeakerIconCoilWidth, Dimensions::SpeakerIconCoilHeight);
        speaker.lineTo(Dimensions::SpeakerIconConeRightX, Dimensions::SpeakerIconConeEndY);
        speaker.lineTo(Dimensions::SpeakerIconConeLeftX, Dimensions::SpeakerIconConeEndY);
        speaker.lineTo(Dimensions::SpeakerIconCoilStartX, Dimensions::SpeakerIconCoilHeight);
        speaker.closeSubPath();

        return speaker;
    }
} // ananas::WFS

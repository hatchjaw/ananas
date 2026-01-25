#include "WFSInterface.h"
#include <AnanasUtils.h>
#include "../Utils.h"

namespace ananas::WFS
{
    WFSInterface::WFSInterface(
        const uint numSources,
        const uint numModules,
        juce::AudioProcessorValueTreeState &apvts,
        juce::ValueTree &persistentTreeToListenTo
    ) : xyController(numSources, apvts),
        persistentTree(persistentTreeToListenTo)
    {
        addAndMakeVisible(xyController);

        addAndMakeVisible(speakerSpacingLabel);
        speakerSpacingLabel.setColour(juce::Label::textColourId, juce::Colours::black);
        speakerSpacingLabel.attachToComponent(&speakerSpacingSlider, false);
        speakerSpacingLabel.setText(Params::SpeakerSpacing.name, juce::dontSendNotification);
        speakerSpacingLabel.setJustificationType(juce::Justification::centredRight);

        addAndMakeVisible(speakerSpacingSlider);
        speakerSpacingSlider.setSliderStyle(juce::Slider::SliderStyle::IncDecButtons);
        speakerSpacingSlider.setNormalisableRange(Params::SpeakerSpacing.rangeDouble);
        speakerSpacingSlider.setValue(Params::SpeakerSpacing.defaultValue);
        speakerSpacingSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,
                                             false,
                                             speakerSpacingSlider.getTextBoxWidth(),
                                             25);
        speakerSpacingSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
        speakerSpacingSlider.setIncDecButtonsMode(juce::Slider::incDecButtonsDraggable_Vertical);

        speakerSpacingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts,
            Params::SpeakerSpacing.id,
            speakerSpacingSlider
        );

        for (uint n{0}; n < numModules; ++n) {
            const auto m{moduleSelectors.add(new ModuleSelector(n, persistentTree))};
            addAndMakeVisible(m);
            m->setBroughtToFrontOnMouseClick(true);
        }

        persistentTree.addListener(this);

        updateModuleLists(persistentTree[ananas::Identifiers::ModulesParamID]);
    }

    WFSInterface::~WFSInterface()
    {
        persistentTree.removeListener(this);
    }

    void WFSInterface::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    void WFSInterface::resized()
    {
        auto bounds{getLocalBounds()};
        auto spacingRow{
            bounds.removeFromTop(Constants::UI::SpeakerSpacingSectionHeight)
            .reduced(6, 0)
        };
        speakerSpacingSlider.setBounds(spacingRow.removeFromRight(100).reduced(1, 3));
        speakerSpacingLabel.setBounds(spacingRow.removeFromRight(200));
        xyController.setBounds(bounds.reduced(10));

        const auto moduleSelectorWidth{xyController.getWidth() / moduleSelectors.size()};
        for (int i{0}; i < moduleSelectors.size(); ++i) {
            moduleSelectors[i]->setBounds(
                xyController.getX() + i * moduleSelectorWidth,
                Constants::UI::SpeakerSpacingSectionHeight + xyController.getHeight() / 2,
                moduleSelectorWidth - 1,
                Constants::UI::ModuleSelectorHeight
            );
        }
    }

    void WFSInterface::updateModuleLists(const juce::var &var)
    {
        juce::StringArray ips;
        if (auto *obj = var.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                auto module{obj->getProperty(prop.name)};
                if (module.getProperty(ananas::Identifiers::ModuleIsConnectedPropertyID, false)) {
                    ips.add(prop.name.toString());
                }
            }
        }
        for (const auto &m: moduleSelectors) {
            m->setAvailableModules(ips);
        }
    }

    void WFSInterface::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
    {
        if (!isVisible()) return;

        if (property == ananas::Identifiers::ModulesParamID) {
            updateModuleLists(treeWhosePropertyHasChanged[property]);
        }
    }
} // ananas::WFS

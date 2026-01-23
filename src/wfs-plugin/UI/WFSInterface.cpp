#include "WFSInterface.h"
#include "../Utils.h"

namespace ananas::WFS
{
    WFSInterface::WFSInterface(
        const uint numSources,
        const uint numModules,
        juce::AudioProcessorValueTreeState &apvts,
        juce::ValueTree &persistentTree
    ) : xyController(numSources, apvts)
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
            const auto cb{moduleSelectors.add(new ModuleSelector(n, persistentTree))};
            addAndMakeVisible(cb);
        }
    }

    void WFSInterface::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    void WFSInterface::resized()
    {
        auto bounds{getLocalBounds()};
        auto spacingRow{
            bounds.removeFromTop(WFS::Constants::UI::SpeakerSpacingSectionHeight)
            .reduced(6, 0)
        };
        speakerSpacingSlider.setBounds(spacingRow.removeFromRight(100).reduced(1, 3));
        speakerSpacingLabel.setBounds(spacingRow.removeFromRight(200));
        xyController.setBounds(bounds.reduced(10));
    }
} // ananas::WFS

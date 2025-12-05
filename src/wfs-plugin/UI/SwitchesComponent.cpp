#include "SwitchesComponent.h"
#include "../Utils.h"

namespace ananas
{
    SwitchesComponent::SwitchesComponent() : addSwitchButton(WFS::Strings::AddSwitchButtonName,
                                                             juce::Colours::black,
                                                             juce::Colours::darkgrey,
                                                             juce::Colours::grey)
    {
        addAndMakeVisible(title);
        addAndMakeVisible(addSwitchButton);

        title.setColour(juce::Label::textColourId, juce::Colours::black);
        title.setFont(juce::Font(juce::FontOptions(15.f, juce::Font::bold)));
        title.setJustificationType(juce::Justification::bottomLeft);
        title.setText(WFS::Strings::SwitchesSectionTitle, juce::dontSendNotification);
    }

    void SwitchesComponent::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    void SwitchesComponent::resized()
    {
        auto bounds{getLocalBounds()};
        title.setBounds(bounds.removeFromTop(48).reduced(5));
        addSwitchButton.setBounds(bounds);
    }
} // ananas

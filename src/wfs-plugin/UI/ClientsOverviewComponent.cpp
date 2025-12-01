#include "ClientsOverviewComponent.h"

#include <AnanasUtils.h>

namespace ananas
{
    ClientsOverviewComponent::ClientsOverviewComponent()
    {
        addAndMakeVisible(title);
        addAndMakeVisible(overviewPanel);
        addAndMakeVisible(clientTable);

        title.setColour(juce::Label::textColourId, juce::Colours::black);
        title.setFont(juce::Font(juce::FontOptions(15.f, juce::Font::bold)));
        title.setJustificationType(juce::Justification::bottomLeft);
        title.setText(WFS::Strings::ClientsSectionTitle, juce::dontSendNotification);
    }

    void ClientsOverviewComponent::update(const juce::var &var)
    {
        if (!isVisible()) return;

        clientTable.update(var);
        overviewPanel.update(var);
    }

    void ClientsOverviewComponent::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    void ClientsOverviewComponent::resized()
    {
        auto bounds{getLocalBounds()};
        title.setBounds(bounds.removeFromTop(48).reduced(5));
        overviewPanel.setBounds(bounds.removeFromTop(35));
        // Client table gets remaining bounds.
        clientTable.setBounds(bounds);
    }
}

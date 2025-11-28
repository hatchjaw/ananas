#include "ClientsOverviewComponent.h"

#include <AnanasUtils.h>

namespace ananas
{
    ClientsOverviewComponent::ClientsOverviewComponent()
    {
        addAndMakeVisible(overviewPanel);
        addAndMakeVisible(clientTable);
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
        overviewPanel.setBounds(bounds.removeFromTop(50));
        // Client table gets remaining bounds.
        clientTable.setBounds(bounds);
    }
}

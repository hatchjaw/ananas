#include "NetworkOverviewComponent.h"

#include <AnanasUtils.h>

ananas::NetworkOverviewComponent::NetworkOverviewComponent()
{
    addAndMakeVisible(switches);
    addAndMakeVisible(timeAuthority);
    addAndMakeVisible(clientOverview);
}

void ananas::NetworkOverviewComponent::update(juce::ValueTree &tree, const juce::Identifier &property)
{
    if (!isVisible()) return;

    if (property == Constants::ConnectedClientsParamID) {
        clientOverview.update(tree[property]);
    } else if (property == Constants::TimeAuthorityParamID) {
        timeAuthority.update(tree[property]);
    }
}

void ananas::NetworkOverviewComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::transparentWhite);
}

void ananas::NetworkOverviewComponent::resized()
{
    auto bounds{getLocalBounds()};
    switches.setBounds(bounds.removeFromTop(150));
    timeAuthority.setBounds(bounds.removeFromTop(120));
    // Client overview gets remaining bounds.
    clientOverview.setBounds(bounds);
}

#include "NetworkOverviewComponent.h"

#include <AnanasUtils.h>

ananas::NetworkOverviewComponent::NetworkOverviewComponent(juce::ValueTree &dynamicTree, juce::ValueTree &persistentTree)
    : switches(dynamicTree, persistentTree),
      timeAuthority(dynamicTree),
      clientOverview(dynamicTree)
{
    addAndMakeVisible(switches);
    addAndMakeVisible(timeAuthority);
    addAndMakeVisible(clientOverview);
}

void ananas::NetworkOverviewComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::transparentWhite);
}

void ananas::NetworkOverviewComponent::resized()
{
    auto bounds{getLocalBounds()};
    switches.setBounds(bounds.removeFromTop(WFS::Constants::SwitchesSectionHeight));
    timeAuthority.setBounds(bounds.removeFromTop(WFS::Constants::TimeAuthoritySectionHeight));
    // Client overview gets remaining bounds.
    clientOverview.setBounds(bounds);
}

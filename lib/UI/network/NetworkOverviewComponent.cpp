#include "NetworkOverviewComponent.h"

namespace ananas::UI {

NetworkOverviewComponent::NetworkOverviewComponent(juce::ValueTree &dynamicTree, juce::ValueTree &persistentTree)
    : switches(dynamicTree, persistentTree),
      timeAuthority(dynamicTree),
      clientOverview(dynamicTree)
{
    addAndMakeVisible(switches);
    addAndMakeVisible(timeAuthority);
    addAndMakeVisible(clientOverview);
}

void NetworkOverviewComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::transparentWhite);
}

void NetworkOverviewComponent::resized()
{
    auto bounds{getLocalBounds()};
    switches.setBounds(bounds.removeFromTop(Dimensions::SwitchesSectionHeight));
    timeAuthority.setBounds(bounds.removeFromTop(Dimensions::TimeAuthoritySectionHeight));
    // Client overview gets remaining bounds.
    clientOverview.setBounds(bounds);

#if SHOW_NO_NETWORK_OVERLAY
    OverlayableComponent::resized();
#endif
}
}

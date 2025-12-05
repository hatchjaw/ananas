#ifndef NETWORKOVERVIEWCOMPONENT_H
#define NETWORKOVERVIEWCOMPONENT_H

#include "SwitchesComponent.h"
#include "TimeAuthorityComponent.h"
#include "ClientsOverviewComponent.h"

namespace ananas
{
    class NetworkOverviewComponent final : public juce::Component
    {
    public:
        NetworkOverviewComponent();

        void update(juce::ValueTree &tree, const juce::Identifier &property);

        void paint(juce::Graphics &g) override;

        void resized() override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NetworkOverviewComponent)

        SwitchesComponent switches;
        TimeAuthorityComponent timeAuthority;
        ClientsOverviewComponent clientOverview;
    };
}



#endif //NETWORKOVERVIEWCOMPONENT_H

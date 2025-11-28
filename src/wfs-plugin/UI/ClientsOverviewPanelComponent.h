#ifndef CLIENTSOVERVIEWPANELCOMPONENT_H
#define CLIENTSOVERVIEWPANELCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace ananas
{
    class ClientsOverviewPanelComponent final : public juce::Component
    {
    public:
        ClientsOverviewPanelComponent();

        void update(const juce::var &var);

        void paint(juce::Graphics &g) override;

        void resized() override;

    private:
        juce::Label totalClientsLabel;
        juce::Label totalClientsValue;
        juce::Label presentationTimeIntervalLabel;
        juce::Label presentationTimeIntervalValue;
    };
}


#endif //CLIENTSOVERVIEWPANELCOMPONENT_H

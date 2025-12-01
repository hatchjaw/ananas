#ifndef CLIENTSOVERVIEWCOMPONENT_H
#define CLIENTSOVERVIEWCOMPONENT_H

#include "ClientsOverviewPanelComponent.h"
#include "ClientTableComponent.h"

namespace ananas
{
    class ClientsOverviewComponent final : public juce::Component
    {
    public:
        ClientsOverviewComponent();

        void update(const juce::var &var);

        void paint(juce::Graphics &g) override;

        void resized() override;

    private:
        juce::Label title;
        ClientsOverviewPanelComponent overviewPanel;
        ClientTableComponent clientTable;
    };
}


#endif //CLIENTSOVERVIEWCOMPONENT_H

#ifndef TABBEDVIEW_H
#define TABBEDVIEW_H
#include <juce_gui_basics/juce_gui_basics.h>

#include "LookAndFeel.h"
#include "../PluginProcessor.h"
#include "network/NetworkOverviewComponent.h"
#include "wfs/WFSInterfaceComponent.h"

namespace ananas::WFS
{
    class TabbedView final : public juce::Component,
                             public juce::ApplicationCommandTarget
    {
    public:
        explicit TabbedView(PluginProcessor &p);

        ~TabbedView() override;

        void resized() override;

        void visibilityChanged() override;

        bool keyPressed(const juce::KeyPress &key) override;

        ApplicationCommandTarget *getNextCommandTarget() override;

        void getAllCommands(juce::Array<int> &commands) override;

        void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;

        bool perform(const InvocationInfo &info) override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TabbedView)

        LookAndFeel lookAndFeel;
        juce::TabbedComponent tabbedComponent{juce::TabbedButtonBar::TabsAtTop};
        NetworkOverviewComponent networkOverview;
        WFSInterfaceComponent wfsInterface;
        juce::ApplicationCommandManager commandManager;
    };
} // ananas::WFS

#endif //TABBEDVIEW_H

#ifndef TABBEDVIEW_H
#define TABBEDVIEW_H
#include <juce_gui_basics/juce_gui_basics.h>

namespace ananas::WFS
{
    class TabbedView final : public juce::Component,
                             public juce::ApplicationCommandTarget
    {
    public:
        ApplicationCommandTarget *getNextCommandTarget() override;

        void getAllCommands(juce::Array<int> &commands) override;

        void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;

        bool perform(const InvocationInfo &info) override;

    private:
        juce::TabbedComponent tabbedComponent{juce::TabbedButtonBar::TabsAtTop};
    };
} // ananas::WFS

#endif //TABBEDVIEW_H

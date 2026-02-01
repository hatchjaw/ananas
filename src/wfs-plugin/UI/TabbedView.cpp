#include "TabbedView.h"

#include "../Utils.h"

namespace ananas::WFS {
    juce::ApplicationCommandTarget *TabbedView::getNextCommandTarget()
    {
        return nullptr;
    }

    void TabbedView::getAllCommands(juce::Array<int> &commands)
    {
        return commands.addArray({
            SwitchToWfsTab,
            SwitchToNetworkTab
        });
    }

    void TabbedView::getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo &result)
    {
        switch (commandID) {
            case SwitchToWfsTab:
                result.setInfo("Switch to WFS Control tab", "Switches to the WFS Control tab", "Tabs", 0);
            result.addDefaultKeypress('1', juce::ModifierKeys::ctrlModifier);
            break;

            case SwitchToNetworkTab:
                result.setInfo("Switch to Network Overview tab", "Switches to the Network Overview tab", "Tabs", 0);
            result.addDefaultKeypress('2', juce::ModifierKeys::ctrlModifier);
            break;

            default:
                break;
        }
    }

    bool TabbedView::perform(const InvocationInfo &info)
    {
        switch (info.commandID) {
            case SwitchToWfsTab:
                tabbedComponent.setCurrentTabIndex(0);
            return true;

            case SwitchToNetworkTab:
                tabbedComponent.setCurrentTabIndex(1);
            return true;

            default:
                return false;
        }
    }
} // ananas::WFS

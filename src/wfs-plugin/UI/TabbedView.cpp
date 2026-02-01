#include "TabbedView.h"

#include "../Utils.h"

namespace ananas::WFS
{
    TabbedView::TabbedView(PluginProcessor &p)
        : networkOverview(
              p.getDynamicTree(),
              p.getPersistentTree()
          ),
          wfsInterface(
              Constants::MaxChannelsToSend,
              Constants::NumModules,
              p.getParamState(),
              p.getPersistentTree()
          )
    {
        setLookAndFeel(&lookAndFeel);

        addAndMakeVisible(tabbedComponent);
        tabbedComponent.addTab(Strings::WfsTabName, juce::Colours::lightgrey, &wfsInterface, false);
        tabbedComponent.addTab(Strings::NetworkTabName, juce::Colours::lightgrey, &networkOverview, false);
        lookAndFeel.setNumberOfTabs(tabbedComponent.getNumTabs());

        commandManager.setFirstCommandTarget(this);
        commandManager.registerAllCommandsForTarget(this);
        addKeyListener(commandManager.getKeyMappings());
        setWantsKeyboardFocus(true);
    }

    TabbedView::~TabbedView()
    {
        setLookAndFeel(nullptr);
    }

    void TabbedView::resized()
    {
        lookAndFeel.setTotalWidth(getWidth());
        tabbedComponent.setBounds(getLocalBounds());
        tabbedComponent.setLookAndFeel(&lookAndFeel);
    }

    void TabbedView::visibilityChanged()
    {
        if (isShowing()) {
            grabKeyboardFocus();
        }
    }

    bool TabbedView::keyPressed(const juce::KeyPress &key)
    {
        DBG("Key pressed: " + key.getTextDescription());
        return false; // Return false so it still propagates
    }

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

#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p),
      tooltipWindow(this, ananas::WFS::Constants::UI::TooltipDelayTimeMs),
      networkOverview(
          getProcessor().getDynamicTree(),
          getProcessor().getPersistentTree()
      ),
      wfsInterface(
          ananas::WFS::Constants::MaxChannelsToSend,
          ananas::WFS::Constants::NumModules,
          getProcessor().getParamState(),
          getProcessor().getPersistentTree(),
          getProcessor().getSourceAmplitudes()
      )
{
    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(tabbedComponent);
    tabbedComponent.addTab(ananas::WFS::Strings::WfsTabName, juce::Colours::lightgrey, &wfsInterface, false);
    tabbedComponent.addTab(ananas::WFS::Strings::NetworkTabName, juce::Colours::lightgrey, &networkOverview, false);
    lookAndFeel.setNumberOfTabs(tabbedComponent.getNumTabs());

    setSize(ananas::WFS::Constants::UI::UiWidth, ananas::WFS::Constants::UI::UiHeight);

    getProcessor().getPersistentTree().addListener(this);
    getProcessor().getDynamicTree().addListener(this);

#ifdef SHOW_NO_NETWORK_OVERLAY
    getProcessor().getServer().addChangeListener(&wfsInterface);
    getProcessor().getServer().addChangeListener(&networkOverview);
#endif

    commandManager.setFirstCommandTarget(this);
    commandManager.registerAllCommandsForTarget(this);
    addKeyListener(commandManager.getKeyMappings());
    setWantsKeyboardFocus(true);
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel(nullptr);

    getProcessor().getPersistentTree().removeListener(this);
    getProcessor().getDynamicTree().removeListener(this);

#ifdef SHOW_NO_NETWORK_OVERLAY
    getProcessor().getServer().removeChangeListener(&wfsInterface);
    getProcessor().getServer().removeChangeListener(&networkOverview);
#endif
}

void PluginEditor::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    lookAndFeel.setTotalWidth(getWidth());
    tabbedComponent.setBounds(getLocalBounds());
    tabbedComponent.setLookAndFeel(&lookAndFeel);
}

void PluginEditor::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
{
    if (property == ananas::Identifiers::SwitchesParamID) {
        getProcessor().getServer().getSwitches()->handleEdit(treeWhosePropertyHasChanged[property]);
    } else if (property == ananas::Identifiers::ClientsShouldRebootParamID) {
        getProcessor().getServer().getClientList()->setShouldReboot(treeWhosePropertyHasChanged[property]);
        treeWhosePropertyHasChanged.setPropertyExcludingListener(this, property, false, nullptr);
    }
}

juce::ApplicationCommandTarget *PluginEditor::getNextCommandTarget()
{
    return nullptr;
}

void PluginEditor::getAllCommands(juce::Array<int> &commands)
{
    return commands.addArray({
        ananas::WFS::SwitchToWfsTab,
        ananas::WFS::SwitchToNetworkTab,
        ananas::WFS::ToggleModuleSelectorDisplay
    });
}

void PluginEditor::getCommandInfo(const juce::CommandID commandID, juce::ApplicationCommandInfo &result)
{
    switch (commandID) {
        case ananas::WFS::SwitchToWfsTab:
            result.setInfo(
                ananas::WFS::Strings::SwitchToWfsTabShortName,
                ananas::WFS::Strings::SwitchToWfsTabDescription,
                ananas::WFS::Strings::TabsCommandCategoryName,
                0);
            result.addDefaultKeypress(ananas::WFS::Constants::UI::SwitchToWfsTabKeycode, juce::ModifierKeys::noModifiers);
            break;

        case ananas::WFS::SwitchToNetworkTab:
            result.setInfo(
                ananas::WFS::Strings::SwitchToNetworkTabShortName,
                ananas::WFS::Strings::SwitchToNetworkTabDescription,
                ananas::WFS::Strings::TabsCommandCategoryName,
                0);
            result.addDefaultKeypress(ananas::WFS::Constants::UI::SwitchToNetworkTabKeycode, juce::ModifierKeys::noModifiers);
            break;

        case ananas::WFS::ToggleModuleSelectorDisplay:
            result.setInfo(
                ananas::WFS::Strings::ToggleModuleSelectorsShortName,
                ananas::WFS::Strings::ToggleModuleSelectorsDescription,
                ananas::WFS::Strings::OptionsCommandCategoryName,
                0);
            result.addDefaultKeypress(ananas::WFS::Constants::UI::ToggleModuleSelectorsKeycode, juce::ModifierKeys::noModifiers);

        default:
            break;
    }
}

bool PluginEditor::perform(const InvocationInfo &info)
{
    switch (info.commandID) {
        case ananas::WFS::SwitchToWfsTab:
            tabbedComponent.setCurrentTabIndex(0);
            return true;

        case ananas::WFS::SwitchToNetworkTab:
            tabbedComponent.setCurrentTabIndex(1);
            return true;

        case ananas::WFS::ToggleModuleSelectorDisplay: {
            const auto showModuleSelectors{getProcessor().getParamState().getParameter(ananas::WFS::Params::ShowModuleSelectors.id)};
            showModuleSelectors->setValueNotifyingHost(fabsf(showModuleSelectors->getValue() - 1.f));
            return true;
        }

        default:
            return false;
    }
}

PluginProcessor &PluginEditor::getProcessor()
{
    return dynamic_cast<PluginProcessor &>(processor);
}

const PluginProcessor &PluginEditor::getProcessor() const
{
    return dynamic_cast<const PluginProcessor &>(processor);
}

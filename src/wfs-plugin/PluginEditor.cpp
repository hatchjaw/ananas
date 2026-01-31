#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p),
      networkOverview(
          getProcessor().getDynamicTree(),
          getProcessor().getPersistentTree()
      ),
      wfsInterface(
          ananas::WFS::Constants::MaxChannelsToSend,
          ananas::WFS::Constants::NumModules,
          getProcessor().getParamState(),
          getProcessor().getPersistentTree()
      ),
      tooltipWindow(this, ananas::WFS::Constants::UI::TooltipDelayTimeMs)
{
    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(tabbedComponent);
    tabbedComponent.addTab(ananas::WFS::Strings::WfsTabName, juce::Colours::lightgrey, &wfsInterface, false);
    tabbedComponent.addTab(ananas::WFS::Strings::NetworkTabName, juce::Colours::lightgrey, &networkOverview, false);
    lookAndFeel.setNumberOfTabs(tabbedComponent.getNumTabs());

    setSize(ananas::WFS::Constants::UI::UiWidth, ananas::WFS::Constants::UI::UiHeight);

    getProcessor().getPersistentTree().addListener(this);
    getProcessor().getDynamicTree().addListener(this);

    getProcessor().getServer().addChangeListener(&wfsInterface);
    getProcessor().getServer().addChangeListener(&networkOverview);

    setWantsKeyboardFocus(false);
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel(nullptr);

    getProcessor().getPersistentTree().removeListener(this);
    getProcessor().getDynamicTree().removeListener(this);

    getProcessor().getServer().removeChangeListener(&wfsInterface);
    getProcessor().getServer().removeChangeListener(&networkOverview);
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

PluginProcessor &PluginEditor::getProcessor()
{
    return dynamic_cast<PluginProcessor &>(processor);
}

const PluginProcessor &PluginEditor::getProcessor() const
{
    return dynamic_cast<const PluginProcessor &>(processor);
}

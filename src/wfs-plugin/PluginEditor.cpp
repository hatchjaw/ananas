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
      )
{
    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(tabbedComponent);
    tabbedComponent.addTab(ananas::WFS::Strings::WfsTabName, juce::Colours::lightgrey, &wfsInterface, false);
    tabbedComponent.addTab(ananas::WFS::Strings::NetworkTabName, juce::Colours::lightgrey, &networkOverview, false);

    setSize(ananas::WFS::Constants::UI::UiWidth, ananas::WFS::Constants::UI::UiHeight);

    getProcessor().getPersistentTree().addListener(this);
    getProcessor().getDynamicTree().addListener(this);

    setWantsKeyboardFocus(false);
}

PluginEditor::~PluginEditor()
{
    getProcessor().getPersistentTree().removeListener(this);
    getProcessor().getDynamicTree().removeListener(this);
    setLookAndFeel(nullptr);
}

void PluginEditor::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    tabbedComponent.setBounds(getLocalBounds());
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
    return static_cast<PluginProcessor &>(processor);
}

const PluginProcessor &PluginEditor::getProcessor() const
{
    return static_cast<const PluginProcessor &>(processor);
}

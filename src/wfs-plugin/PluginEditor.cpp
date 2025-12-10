#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p),
      networkOverview(getProcessor().getDynamicTree(), getProcessor().getPersistentTree())
{
    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(tabbedComponent);
    tabbedComponent.addTab(ananas::WFS::Strings::WfsTabName, juce::Colours::lightgrey, nullptr, false);
    tabbedComponent.addTab(ananas::WFS::Strings::NetworkTabName, juce::Colours::lightgrey, &networkOverview, false);

    setSize(ananas::WFS::Constants::UiWidth, ananas::WFS::Constants::UiHeight);

    getProcessor().getPersistentTree().addListener(this);
}

PluginEditor::~PluginEditor()
{
    getProcessor().getPersistentTree().removeListener(this);
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
    getProcessor().getServer().getSwitches()->handleEdit(treeWhosePropertyHasChanged[property]);
}

PluginProcessor &PluginEditor::getProcessor()
{
    return static_cast<PluginProcessor &>(processor);
}

const PluginProcessor &PluginEditor::getProcessor() const
{
    return static_cast<const PluginProcessor &>(processor);
}

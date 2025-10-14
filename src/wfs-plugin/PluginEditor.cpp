#include "PluginEditor.h"

#include <X11/X.h>

PluginEditor::PluginEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p)
{
    setLookAndFeel(&lookAndFeel);

    addAndMakeVisible(tabbedComponent);
    tabbedComponent.addTab(ananas::WFS::Strings::ClientsTabName, juce::Colours::lightgrey, &clientTable, false);

    getProcessor().getDynamicTree().addListener(this);

    setSize(1200, 900);
}

PluginEditor::~PluginEditor()
{
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

void PluginEditor::parameterChanged(const juce::String &parameterID, float newValue)
{
    ignoreUnused(parameterID, newValue);
}

void PluginEditor::valueTreePropertyChanged(juce::ValueTree &tree, const juce::Identifier &property)
{
    if (property == ananas::Constants::ConnectedClientsParamID) {
        clientTable.update(tree[property]);
        triggerAsyncUpdate();
    }
}

void PluginEditor::handleAsyncUpdate()
{
    repaint();
}

PluginProcessor &PluginEditor::getProcessor()
{
    return static_cast<PluginProcessor &>(processor);
}

const PluginProcessor &PluginEditor::getProcessor() const
{
    return static_cast<const PluginProcessor &>(processor);
}

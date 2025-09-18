#include "PluginEditor.h"

PluginEditor::PluginEditor(PluginProcessor &p,
                           juce::AudioProcessorValueTreeState &apvts,
                           juce::ValueTree &vt)
    : AudioProcessorEditor(&p),
      processor(p),
      staticTree(apvts),
      dynamicTree(vt)
{
    setSize(1200, 900);
}

PluginEditor::~PluginEditor()
{
    // settingsWindow.deleteAndZero();
    // dynamicTree.removeListener(settingsComponent.get());
    setLookAndFeel(nullptr);
}

void PluginEditor::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    AudioProcessorEditor::resized();
}

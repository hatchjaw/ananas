#include "PluginEditor.h"

#include <X11/X.h>

PluginEditor::PluginEditor(PluginProcessor &p)
    : AudioProcessorEditor(&p)
{
    setSize(1200, 900);

    getProcessor().getDynamicTree().addListener(this);
}

PluginEditor::~PluginEditor()
{
    getProcessor().getDynamicTree().removeListener(this);
    setLookAndFeel(nullptr);
}

void PluginEditor::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    auto y{100};
    const auto clients{getProcessor().getDynamicTree().getProperty(ananas::Constants::ConnectedClientsParamID).getArray()};
    if (clients != nullptr) {
        g.setColour(juce::Colours::white);
        g.setFont(15.0f);
        for (auto *it{clients->begin()}; it < clients->end(); ++it, y += 50) {
            const auto samplingRate{static_cast<float>(it->getProperty("samplingRate", 0))};
            const auto percentCPU{static_cast<float>(it->getProperty("percentCPU", 0))};
            g.drawSingleLineText(
                juce::String{
                    "IP: " + it->getProperty("ip", "-.-.-.-").toString() +
                    ", serial: " + it->getProperty("serial", "--------").toString() +
                    ", offset: " + it->getProperty("offsetTime", "-.-.-.-.").toString() +
                    " ns (" + it->getProperty("offsetFrame", 0).toString() + " frames)" +
                    ", packet buffer " + it->getProperty("bufferFillPercent", 0).toString() + " % full" +
                    ", sampling rate " + juce::String(samplingRate, 6) + " Hz" +
                    ", " + juce::String(percentCPU, 3) + " % CPU"
                }, 100, y
            );
        }
    }
}

void PluginEditor::resized()
{
}

void PluginEditor::parameterChanged(const juce::String &parameterID, float newValue)
{
    ignoreUnused(parameterID, newValue);
}

void PluginEditor::valueTreePropertyChanged(juce::ValueTree &tree, const juce::Identifier &property)
{
    ignoreUnused(tree);

    if (property == ananas::Constants::ConnectedClientsParamID) {
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

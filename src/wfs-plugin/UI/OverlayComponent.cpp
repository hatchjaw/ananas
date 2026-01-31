#include "OverlayComponent.h"

namespace ananas {
    void OverlayComponent::paint(juce::Graphics &g)
    {
        // Semi-transparent background over entire component
        g.fillAll(juce::Colours::black.withAlpha(0.5f));

        // Calculate centered box
        auto boxWidth = 300;
        auto boxHeight = 150;
        auto box = juce::Rectangle<int>(boxWidth, boxHeight)
            .withCentre(getLocalBounds().getCentre());

        // Draw opaque box
        g.setColour(juce::Colours::white);
        g.fillRoundedRectangle(box.toFloat(), 10.0f);

        // Optional: add border
        g.setColour(juce::Colours::darkgrey);
        g.drawRoundedRectangle(box.toFloat(), 10.0f, 2.0f);

        // Draw text
        g.setColour(juce::Colours::black);
        g.setFont(16.0f);
        g.drawText("No network connection", box, juce::Justification::centred);
    }

    void OverlayComponent::setText(const juce::String &textToDisplay)
    {
        text = textToDisplay;
    }
} // ananas

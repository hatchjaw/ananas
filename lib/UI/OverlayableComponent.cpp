#include "OverlayableComponent.h"
#include "AnanasUIUtils.h"
#include <Server.h>

namespace ananas::UI
{
    OverlayableComponent::OverlayableComponent()
    {
        // Always create the overlay to begin with.
        overlay = std::make_unique<OverlayComponent>();
        addAndMakeVisible(overlay.get(), -1);
        overlay->setText(Strings::OverlayInitialText);
        overlay->toFront(true);
    }

    void OverlayableComponent::changeListenerCallback(juce::ChangeBroadcaster *source)
    {
        if (!isVisible()) return;

        if (const auto *server = dynamic_cast<Server *>(source)) {
            const auto connected{server->isConnected()};

            // If the server is connected and the overlay is present, get rid
            // of the overlay.
            if (connected && overlay != nullptr) {
                overlay.reset();
            } else if (!connected) {
                // If not connected and there's no overlay, create the overlay
                // with text indicating lack of connection.
                if (overlay != nullptr) {
                    overlay = std::make_unique<OverlayComponent>();
                    addAndMakeVisible(overlay.get(), -1);
                    overlay->setText(Strings::OverlayNoNetworkText);
                }
                // Resize to trigger a redraw.
                resized();
            }
        }
    }

    void OverlayableComponent::resized()
    {
        if (overlay != nullptr) {
            overlay->setBounds(getLocalBounds());
            overlay->toFront(true);
        }
    }

    //==========================================================================

    void OverlayableComponent::OverlayComponent::paint(juce::Graphics &g)
    {
        // Semi-transparent background
        g.fillAll(juce::Colours::black.withAlpha(Colours::OverlayBgAlpha));

        // Calculate centered box
        const auto box{
            juce::Rectangle(Dimensions::OverlayBoxWidth, Dimensions::OverlayBoxHeight)
            .withCentre(getLocalBounds().getCentre())
        };

        // Create shadow
        const juce::DropShadow shadow(
            juce::Colours::black.withAlpha(Colours::OverlayBoxShadowAlpha),
            Dimensions::OverlayBoxBorderRadius * 2.f,
            juce::Point(Dimensions::OverlayBoxShadowOffset,
                        Dimensions::OverlayBoxShadowOffset));

        // Draw the shadow
        shadow.drawForRectangle(g, box);

        // Draw opaque box
        g.setColour(juce::Colours::white);
        g.fillRoundedRectangle(box.toFloat(), Dimensions::OverlayBoxBorderRadius);

        // Add border
        g.setColour(juce::Colours::slategrey);
        g.drawRoundedRectangle(box.toFloat(), Dimensions::OverlayBoxBorderRadius, Dimensions::OverlayBoxBorderThickness);

        // Draw text
        g.setColour(juce::Colours::black);
        g.setFont(Dimensions::OverlayBoxTextSize);
        g.drawText(text, box, juce::Justification::centred);
    }

    void OverlayableComponent::OverlayComponent::setText(const juce::String &textToDisplay)
    {
        text = textToDisplay;
    }
}

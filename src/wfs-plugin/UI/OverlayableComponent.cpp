#include "OverlayableComponent.h"
#include <Server.h>

namespace ananas {
    void OverlayableComponent::changeListenerCallback(juce::ChangeBroadcaster *source)
    {
        if (const auto *server = dynamic_cast<Server *>(source)) {
            const auto connected{server->isConnected()};
            if (connected && overlay != nullptr) {
                overlay.reset();
            } else if (!connected && overlay == nullptr) {
                overlay = std::make_unique<OverlayComponent>();
                addAndMakeVisible(overlay.get(), -1);
                overlay->toFront(true);
                resized();
            }
        }
    }

    void OverlayableComponent::resized()
    {
        if (overlay != nullptr)
            overlay->setBounds(getLocalBounds());
    }
} // ananas

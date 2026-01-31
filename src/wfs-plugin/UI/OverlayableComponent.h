#ifndef OVERLAYABLECOMPONENT_H
#define OVERLAYABLECOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "OverlayComponent.h"

namespace ananas
{
    class OverlayableComponent : public juce::Component,
                                 public juce::ChangeListener
    {
    public:
        void changeListenerCallback(juce::ChangeBroadcaster *source) override;

        void resized() override;

    private:
        std::unique_ptr<OverlayComponent> overlay;
    };
} // ananas

#endif //OVERLAYABLECOMPONENT_H

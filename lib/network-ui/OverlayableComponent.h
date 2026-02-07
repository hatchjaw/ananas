#ifndef OVERLAYABLECOMPONENT_H
#define OVERLAYABLECOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace ananas::UI
{
    class OverlayableComponent : public juce::Component,
                                 public juce::ChangeListener
    {
    public:
        OverlayableComponent();

        void changeListenerCallback(juce::ChangeBroadcaster *source) override;

        void resized() override;

    private:
        class OverlayComponent final : public Component
        {
        public:
            void paint(juce::Graphics &g) override;

            void setText(const juce::String &textToDisplay);
        private:
            juce::String text;
        };

        std::unique_ptr<OverlayComponent> overlay;
    };
} // ananas

#endif //OVERLAYABLECOMPONENT_H

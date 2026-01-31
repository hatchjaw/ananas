#ifndef OVERLAYCOMPONENT_H
#define OVERLAYCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace ananas
{
    class OverlayComponent final : public juce::Component
    {
    public:
        void paint(juce::Graphics &g) override;

        void setText(const juce::String &textToDisplay);
    private:
        juce::String text;
    };
} // ananas

#endif //OVERLAYCOMPONENT_H

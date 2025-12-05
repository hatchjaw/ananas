#ifndef SWITCHESCOMPONENT_H
#define SWITCHESCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace ananas
{
    class SwitchesComponent final : public juce::Component
    {
    public:
        SwitchesComponent();

        void paint(juce::Graphics &g) override;

        void resized() override;
    private:
        juce::Label title;
        juce::ShapeButton addSwitchButton;
    };
} // ananas

#endif //SWITCHESCOMPONENT_H

#ifndef WFSCONTROLLER_H
#define WFSCONTROLLER_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "ModuleSelector.h"
#include "XYController.h"

namespace ananas::WFS
{
    class WFSInterface : public juce::Component
    {
    public:
        WFSInterface(uint numSources, uint numModules, juce::AudioProcessorValueTreeState &apvts, juce::ValueTree &persistentTree);

        void paint(juce::Graphics &g) override;

        void resized() override;

    private:
        XYController xyController;
        juce::OwnedArray<ModuleSelector> moduleSelectors;
        juce::Slider speakerSpacingSlider;
        juce::Label speakerSpacingLabel;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speakerSpacingAttachment;
    };
} // ananas::WFS

#endif //WFSCONTROLLER_H

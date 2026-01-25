#ifndef WFSCONTROLLER_H
#define WFSCONTROLLER_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "ModuleSelector.h"
#include "XYController.h"

namespace ananas::WFS
{
    class WFSInterface : public juce::Component,
                         public juce::ValueTree::Listener
    {
    public:
        WFSInterface(uint numSources,
                     uint numModules,
                     juce::AudioProcessorValueTreeState &apvts,
                     juce::ValueTree &persistentTreeToListenTo);

        ~WFSInterface();

        void paint(juce::Graphics &g) override;

        void resized() override;

        void updateModuleLists(const juce::var &var);

        void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WFSInterface)

        XYController xyController;
        juce::OwnedArray<ModuleSelector> moduleSelectors;
        juce::Slider speakerSpacingSlider;
        juce::Label speakerSpacingLabel;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speakerSpacingAttachment;
        juce::ValueTree &persistentTree;
    };
} // ananas::WFS

#endif //WFSCONTROLLER_H

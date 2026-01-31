#ifndef WFSCONTROLLERCOMPONENT_H
#define WFSCONTROLLERCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "ModuleSelectorComponent.h"
#include "XYControllerComponent.h"
#include "../OverlayableComponent.h"

namespace ananas::WFS
{
    class WFSInterfaceComponent final : public OverlayableComponent,
                                        public juce::ValueTree::Listener
    {
    public:
        WFSInterfaceComponent(uint numSources,
                              uint numModules,
                              juce::AudioProcessorValueTreeState &apvts,
                              juce::ValueTree &persistentTreeToListenTo);

        ~WFSInterfaceComponent() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

        void updateModuleLists(const juce::var &var);

        void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WFSInterfaceComponent)

        XYControllerComponent xyController;
        juce::OwnedArray<ModuleSelectorComponent> moduleSelectors;
        juce::Slider speakerSpacingSlider;
        juce::Label speakerSpacingLabel;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speakerSpacingAttachment;
        juce::ValueTree &persistentTree;
    };
} // ananas::WFS

#endif //WFSCONTROLLERCOMPONENT_H

#ifndef WFSCONTROLLERCOMPONENT_H
#define WFSCONTROLLERCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "ModuleComponent.h"
#include "XYControllerComponent.h"
#if SHOW_NO_NETWORK_OVERLAY
#include <OverlayableComponent.h>
#endif

namespace ananas::WFS::UI
{
    class WFSInterfaceComponent final :
#if SHOW_NO_NETWORK_OVERLAY
            public ananas::UI::OverlayableComponent,
#else
            public juce::Component,
#endif
            public juce::ValueTree::Listener,
            public juce::AudioProcessorValueTreeState::Listener
    {
    public:
        WFSInterfaceComponent(int numSources,
                              int numModules,
                              juce::AudioProcessorValueTreeState &apvts,
                              juce::ValueTree &persistentTreeToListenTo,
                              juce::HashMap<int, std::atomic<float> *> &sourceAmplitudes);

        ~WFSInterfaceComponent() override;

        void paint(juce::Graphics &g) override;

        void resized() override;

        void updateModuleLists(const juce::var &var);

        void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

        void parameterChanged(const juce::String &parameterID, float newValue) override;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WFSInterfaceComponent)

        juce::AudioProcessorValueTreeState &state;
        XYControllerComponent xyController;
        juce::OwnedArray<ModuleComponent> modules;
        juce::Slider speakerSpacingSlider;
        juce::Label speakerSpacingLabel;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speakerSpacingAttachment;
        juce::ToggleButton showModuleSelectorsButton;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> showModuleSelectorsAttachment;
        juce::ValueTree &persistentTree;
    };
} // ananas::WFS

#endif //WFSCONTROLLERCOMPONENT_H

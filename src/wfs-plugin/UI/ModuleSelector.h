#ifndef MODULESELECTOR_H
#define MODULESELECTOR_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace ananas::WFS
{
    class ModuleSelector : public juce::Component
    {
    public:
        explicit ModuleSelector(uint moduleIndex, juce::ValueTree &persistentTree);

        void paint(juce::Graphics &g) override;

        void resized() override;

        void setAvailableModules(const juce::StringArray &ips);

        void setIndexForModule() const;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuleSelector)

        void setSelectedModule(const juce::var &var);

        juce::ValueTree &tree;
        juce::ComboBox comboBox;
        uint index{0};
    };
} // ananas::WFS

#endif //MODULESELECTOR_H

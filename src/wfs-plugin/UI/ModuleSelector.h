#ifndef MODULESELECTOR_H
#define MODULESELECTOR_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace ananas::WFS
{
    class ModuleSelector : public juce::Component
    {
    public:
        explicit ModuleSelector(uint moduleIndex, juce::ValueTree &persistentTree);

    private:
        juce::ComboBox comboBox;
    };
} // ananas::WFS

#endif //MODULESELECTOR_H

#include "ModuleSelector.h"

#include "../Utils.h"

namespace ananas::WFS {
    ModuleSelector::ModuleSelector(const uint moduleIndex, juce::ValueTree &persistentTree)
    {
        addAndMakeVisible(comboBox);

        comboBox.onChange = [this, moduleIndex, &persistentTree]
        {
            persistentTree.setProperty(Params::getModuleIndexParamID(moduleIndex), comboBox.getText(), nullptr);
        };
    }
} // ananas::WFS

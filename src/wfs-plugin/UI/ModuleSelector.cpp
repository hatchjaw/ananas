#include "ModuleSelector.h"
#include <AnanasUtils.h>
#include "../Utils.h"

namespace ananas::WFS
{
    ModuleSelector::ModuleSelector(const uint moduleIndex, juce::ValueTree &persistentTree)
        : tree(persistentTree),
          index(moduleIndex)
    {
        addAndMakeVisible(comboBox);

        comboBox.onChange = [this]
        {
            setIndexForModule();
        };
    }

    void ModuleSelector::paint(juce::Graphics &g)
    {
        Component::paint(g);
    }

    void ModuleSelector::resized()
    {
        comboBox.setBounds(getLocalBounds());
    }

    void ModuleSelector::setAvailableModules(const juce::StringArray &ips)
    {
        comboBox.clear(juce::dontSendNotification);
        comboBox.addItemList(ips, 1);

        auto modules{tree.getProperty(ananas::Identifiers::ModulesParamID)};
        auto varIntN{juce::var{static_cast<int>(index)}};
        if (auto *obj = modules.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                if (prop.value.getProperty(ananas::Identifiers::ModuleIDPropertyID, 0) == varIntN) {
                    setSelectedModule(prop.name.toString());
                }
            }
        }
    }

    void ModuleSelector::setIndexForModule() const
    {
        auto modules{tree.getProperty(ananas::Identifiers::ModulesParamID)};
        if (auto *obj = modules.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                if (prop.name.toString() == comboBox.getText()) {
                    auto module{obj->getProperty(prop.name).getDynamicObject()};
                    module->setProperty(ananas::Identifiers::ModuleIDPropertyID, static_cast<int>(index));
                    tree.sendPropertyChangeMessage(ananas::Identifiers::ModulesParamID);
                }
            }
        }
    }

    void ModuleSelector::setSelectedModule(const juce::var &var)
    {
        if (var.isString()) {
            comboBox.setText(var.toString(), juce::dontSendNotification);
        }
    }
} // ananas::WFS

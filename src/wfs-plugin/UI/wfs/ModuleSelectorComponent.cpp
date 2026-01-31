#include "ModuleSelectorComponent.h"
#include <AnanasUtils.h>
#include "../../Utils.h"

namespace ananas::WFS
{
    ModuleSelectorComponent::ModuleSelectorComponent(const uint moduleIndex, juce::ValueTree &persistentTree)
        : tree(persistentTree),
          index(moduleIndex)
    {
        addAndMakeVisible(comboBox);

        comboBox.onChange = [this]
        {
            setIndexForModule();
        };
    }

    void ModuleSelectorComponent::paint(juce::Graphics &g)
    {
        Component::paint(g);
    }

    void ModuleSelectorComponent::resized()
    {
        comboBox.setBounds(getLocalBounds());
    }

    void ModuleSelectorComponent::setAvailableModules(const juce::StringArray &ips)
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

    void ModuleSelectorComponent::setIndexForModule() const
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

    void ModuleSelectorComponent::setSelectedModule(const juce::var &var)
    {
        if (var.isString()) {
            comboBox.setText(var.toString(), juce::dontSendNotification);
        }
    }
} // ananas::WFS

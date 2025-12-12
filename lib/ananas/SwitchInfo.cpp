#include "SwitchInfo.h"
#include <AnanasUtils.h>

namespace ananas
{
    void SwitchInfo::update(const juce::var *response)
    {
        info = *response;
        const auto *obj{response->getDynamicObject()};
        if (obj->hasProperty(Identifiers::SwitchClockIdPropertyId)) {
            clockID = obj->getProperty(Identifiers::SwitchClockIdPropertyId);
        }
        if (obj->hasProperty(Identifiers::SwitchFreqDriftPropertyId)) {
            freqDrift = obj->getProperty(Identifiers::SwitchFreqDriftPropertyId);
        }
        if (obj->hasProperty(Identifiers::SwitchOffsetPropertyId)) {
            offset = obj->getProperty(Identifiers::SwitchOffsetPropertyId);
        }
    }

    juce::var SwitchInfo::toVar() const
    {
        const auto object{new juce::DynamicObject()};

        object->setProperty(Identifiers::SwitchIpPropertyID, ip);
        object->setProperty(Identifiers::SwitchUsernamePropertyID, username);
        object->setProperty(Identifiers::SwitchPasswordPropertyID, password);
        object->setProperty(Identifiers::SwitchFreqDriftPropertyId, static_cast<int>(freqDrift));
        object->setProperty(Identifiers::SwitchOffsetPropertyId, static_cast<int>(offset));
        object->setProperty(Identifiers::SwitchShouldResetPtpPropertyID, shouldResetPtp);

        return object;
    }

    juce::ValueTree SwitchInfo::toValueTree() const
    {
        juce::ValueTree tree("Switch");
        tree.setProperty(Identifiers::SwitchIpPropertyID, ip, nullptr);
        tree.setProperty(Identifiers::SwitchUsernamePropertyID, username, nullptr);
        tree.setProperty(Identifiers::SwitchPasswordPropertyID, password, nullptr);
        return tree;
    }

    SwitchInfo SwitchInfo::fromValueTree(const juce::ValueTree &tree)
    {
        SwitchInfo info;
        info.ip = tree.getProperty(Identifiers::SwitchIpPropertyID);
        info.username = tree.getProperty(Identifiers::SwitchUsernamePropertyID);
        info.password = tree.getProperty(Identifiers::SwitchPasswordPropertyID);
        return info;
    }

    //==========================================================================

    void SwitchList::handleEdit(const juce::var &data)
    {
        const auto obj{data.getDynamicObject()};
        for (const auto &prop: obj->getProperties()) {
            if (const auto *s = prop.value.getDynamicObject()) {
                auto index{prop.name.toString().fromLastOccurrenceOf("_", false, false).getIntValue()};

                if (s->getProperty(Identifiers::SwitchShouldRemovePropertyID)) {
                    // TODO erasing switch_0 when, e.g., switch_0 and switch_1
                    //  exist, breaks table logic that relies on switch indices
                    //  matching row numbers; fix this.
                    switches.erase(index);
                    sendChangeMessage();
                    return;
                }

                if (s->getProperty(Identifiers::SwitchShouldResetPtpPropertyID)) {
                    switches.at(index).shouldResetPtp = true;
                    sendChangeMessage();
                    return;
                }

                auto iter{switches.find(index)};
                if (iter == switches.end()) {
                    SwitchInfo i{};
                    iter = switches.insert(std::make_pair(index, i)).first;
                    std::cout << "Adding switch " << iter->first << std::endl;
                }

                iter->second.ip = s->getProperty(Identifiers::SwitchIpPropertyID).toString();
                iter->second.username = s->getProperty(Identifiers::SwitchUsernamePropertyID).toString();
                iter->second.password = s->getProperty(Identifiers::SwitchPasswordPropertyID).toString();
            }
        }
    }

    void SwitchList::handleResponse(const int switchIndex, const juce::var &response)
    {
        if (response.isArray()) {
            // TODO: don't make this dreadful assumption.
            if (response.getArray()->isEmpty()) {
                switches.at(switchIndex).shouldResetPtp = false;
                sendChangeMessage();
                return;
            }

            const auto switchInfo = response.getArray()->getFirst();
            auto iter{switches.find(switchIndex)};
            if (iter == switches.end()) {
                SwitchInfo s{};
                iter = switches.insert(std::make_pair(switchIndex, s)).first;
                std::cout << "Found switch " << iter->first << std::endl;
            }
            iter->second.update(&switchInfo);
            sendChangeMessage();
        } else {
            // Probably got an error response...
        }
    }

    juce::var SwitchList::toVar() const
    {
        const auto object{new juce::DynamicObject()};

        for (const auto &[index, switchInfo]: switches) {
            object->setProperty("switch_" + juce::String{index}, switchInfo.toVar());
        }

        return object;
    }

    juce::ValueTree SwitchList::toValueTree() const
    {
        juce::ValueTree tree(Identifiers::SwitchesParamID);

        for (const auto &[index, switchInfo]: switches) {
            auto switchTree{switchInfo.toValueTree()};
            switchTree.setProperty("index", index, nullptr);
            tree.addChild(switchTree, -1, nullptr);
        }

        return tree;
    }

    void SwitchList::fromValueTree(const juce::ValueTree &tree)
    {
        switches.clear();

        for (int i{0}; i < tree.getNumChildren(); ++i) {
            auto switchTree{tree.getChild(i)};
            int index{switchTree.getProperty("index")};
            switches[index] = SwitchInfo::fromValueTree(switchTree);
        }
    }
}

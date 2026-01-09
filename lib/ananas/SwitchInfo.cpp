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
                if (s->getProperty(Identifiers::SwitchShouldRemovePropertyID)) {
                    std::cout << "Removing " << prop.name.toString() << std::endl;
                    switches.erase(prop.name);//(index);
                    sendChangeMessage();
                    return;
                }

                if (s->getProperty(Identifiers::SwitchShouldResetPtpPropertyID)) {
                    switches.at(prop.name).shouldResetPtp = true;//(index).shouldResetPtp = true;
                    sendChangeMessage();
                    return;
                }

                auto iter{switches.find(prop.name)};//(index)};
                if (iter == switches.end()) {
                    SwitchInfo i{};
                    iter = switches.insert(std::make_pair(prop.name, i)).first;//(index, i)).first;
                    std::cout << "Adding " << iter->first.toString() << std::endl;
                }

                iter->second.ip = s->getProperty(Identifiers::SwitchIpPropertyID).toString();
                iter->second.username = s->getProperty(Identifiers::SwitchUsernamePropertyID).toString();
                iter->second.password = s->getProperty(Identifiers::SwitchPasswordPropertyID).toString();
            }
        }
    }

    void SwitchList::handleResponse(const juce::Identifier &switchID, const juce::var &response)
    {
        if (response.isArray()) {
            // TODO: don't make this dreadful assumption.
            if (response.getArray()->isEmpty()) {
                switches.at(switchID).shouldResetPtp = false;
                sendChangeMessage();
                return;
            }

            const auto switchInfo = response.getArray()->getFirst();
            auto iter{switches.find(switchID)};
            if (iter == switches.end()) {
                SwitchInfo s{};
                iter = switches.insert(std::make_pair(switchID, s)).first;
                std::cout << "Found " << iter->first.toString() << std::endl;
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

        for (const auto &[identifier, switchInfo]: switches) {
            object->setProperty(identifier, switchInfo.toVar());
        }

        return object;
    }

    juce::ValueTree SwitchList::toValueTree() const
    {
        juce::ValueTree tree(Identifiers::SwitchesParamID);

        for (const auto &[identifier, switchInfo]: switches) {
            auto switchTree{switchInfo.toValueTree()};
            switchTree.setProperty("identifier", identifier.toString(), nullptr);
            tree.addChild(switchTree, -1, nullptr);
        }

        return tree;
    }

    void SwitchList::fromValueTree(const juce::ValueTree &tree)
    {
        switches.clear();

        for (int i{0}; i < tree.getNumChildren(); ++i) {
            auto switchTree{tree.getChild(i)};
            juce::Identifier identifier{switchTree.getProperty("identifier")};
            switches[identifier] = SwitchInfo::fromValueTree(switchTree);
        }
    }
}

#include "SwitchInfo.h"
#include "ServerUtils.h"
#include <AnanasUtils.h>

namespace ananas
{
    void SwitchInfo::update(const juce::var *response)
    {
        info = *response;
        const auto *obj{response->getDynamicObject()};
        if (obj->hasProperty(Utils::Identifiers::SwitchClockIdPropertyId)) {
            clockID = obj->getProperty(Utils::Identifiers::SwitchClockIdPropertyId);
        }
        if (obj->hasProperty(Utils::Identifiers::SwitchFreqDriftPropertyId)) {
            freqDrift = obj->getProperty(Utils::Identifiers::SwitchFreqDriftPropertyId);
        }
        if (obj->hasProperty(Utils::Identifiers::SwitchOffsetPropertyId)) {
            offset = obj->getProperty(Utils::Identifiers::SwitchOffsetPropertyId);
        }
    }

    juce::var SwitchInfo::toVar() const
    {
        const auto object{new juce::DynamicObject()};

        object->setProperty(Utils::Identifiers::SwitchIpPropertyID, ip);
        object->setProperty(Utils::Identifiers::SwitchUsernamePropertyID, username);
        object->setProperty(Utils::Identifiers::SwitchPasswordPropertyID, password);
        object->setProperty(Utils::Identifiers::SwitchFreqDriftPropertyId, static_cast<int>(freqDrift));
        object->setProperty(Utils::Identifiers::SwitchOffsetPropertyId, static_cast<int>(offset));
        object->setProperty(Utils::Identifiers::SwitchShouldResetPtpPropertyID, shouldResetPtp);

        return object;
    }

    juce::ValueTree SwitchInfo::toValueTree() const
    {
        juce::ValueTree tree("Switch");
        tree.setProperty(Utils::Identifiers::SwitchIpPropertyID, ip, nullptr);
        tree.setProperty(Utils::Identifiers::SwitchUsernamePropertyID, username, nullptr);
        tree.setProperty(Utils::Identifiers::SwitchPasswordPropertyID, password, nullptr);
        return tree;
    }

    SwitchInfo SwitchInfo::fromValueTree(const juce::ValueTree &tree)
    {
        SwitchInfo info;
        info.ip = tree.getProperty(Utils::Identifiers::SwitchIpPropertyID);
        info.username = tree.getProperty(Utils::Identifiers::SwitchUsernamePropertyID);
        info.password = tree.getProperty(Utils::Identifiers::SwitchPasswordPropertyID);
        return info;
    }

    //==========================================================================

    void SwitchList::handleEdit(const juce::var &data)
    {
        const auto obj{data.getDynamicObject()};
        for (const auto &prop: obj->getProperties()) {
            if (const auto *s = prop.value.getDynamicObject()) {
                if (s->getProperty(Utils::Identifiers::SwitchShouldRemovePropertyID)) {
                    std::cout << "Removing " << prop.name.toString() << std::endl;
                    switches.erase(prop.name);//(index);
                    sendChangeMessage();
                    return;
                }

                if (s->getProperty(Utils::Identifiers::SwitchShouldResetPtpPropertyID)) {
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

                iter->second.ip = s->getProperty(Utils::Identifiers::SwitchIpPropertyID).toString();
                iter->second.username = s->getProperty(Utils::Identifiers::SwitchUsernamePropertyID).toString();
                iter->second.password = s->getProperty(Utils::Identifiers::SwitchPasswordPropertyID).toString();
            }
        }
    }

    void SwitchList::handleResponse(const juce::Identifier &switchID, const juce::var &response)
    {
        if (response.isArray()) {
            // TODO: maybe don't make this dreadful assumption.
            //  Thing is, the switch returns an empty JSON array on (successful)
            //  PTP disable/enable.
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
            // TODO: Probably got an error response...
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
        juce::ValueTree tree(Utils::Identifiers::SwitchesParamID);

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

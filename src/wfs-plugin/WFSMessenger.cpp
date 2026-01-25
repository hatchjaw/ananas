#include "WFSMessenger.h"

#include <AnanasUtils.h>

#include "Utils.h"

namespace ananas::WFS
{
    bool WFSMessenger::prepare()
    {
        if (!socket.bindToPort(Constants::WFSMessengerSocketLocalPort, Constants::LocalInterfaceIP)) {
            std::cerr << "WFS Messenger failed to bind to port: " << std::strerror(errno) << std::endl;
            return false;
        }

        if (!connectToSocket(socket, Constants::WFSControlMulticastIP, Constants::WFSMessengerSocketRemotePort)) {
            std::cerr << "WFS Messenger failed to connect to socket: " << std::strerror(errno) << std::endl;
            return false;
        }

        return true;
    }

    void WFSMessenger::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged,
                                                const juce::Identifier &property)
    {
        if (property == ananas::Identifiers::ModulesParamID) {
            if (auto *obj = treeWhosePropertyHasChanged[property].getDynamicObject()) {
                for (const auto &prop: obj->getProperties()) {
                    auto module{obj->getProperty(prop.name)};
                    auto id{module.getProperty(ananas::Identifiers::ModuleIDPropertyID, 0)};
                    auto path{Params::getModuleIndexParamID(id)};
                    juce::OSCBundle bundle;
                    DBG("Sending OSC: " << path << " " << prop.name);
                    bundle.addElement(juce::OSCMessage{path, prop.name.toString()});
                    send(bundle);
                }
            }
        }
    }

    void WFSMessenger::parameterChanged(const juce::String &parameterID, float newValue)
    {
        juce::OSCBundle bundle;
        DBG("Sending OSC: " << parameterID << " " << newValue);
        bundle.addElement(juce::OSCMessage{parameterID, newValue});
        send(bundle);
    }
} // ananas::WFS

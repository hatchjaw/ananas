#include "WFSMessenger.h"
#include <AnanasUtils.h>
#include "WFSUtils.h"

namespace ananas::WFS
{
    bool WFSMessenger::connect()
    {
        if (!socket.bindToPort(Constants::WFSMessengerSocketLocalPort, Constants::LocalInterfaceIP)) {
            std::cerr << "WFS Messenger failed to bind to port: " << std::strerror(errno) << std::endl;
            return false;
        }

        if (!connectToSocket(socket, Constants::WFSControlMulticastIP, Constants::WFSMessengerSocketRemotePort)) {
            std::cerr << "WFS Messenger failed to connect to socket: " << std::strerror(errno) << std::endl;
            return false;
        }

        connected = true;
        return true;
    }

    void WFSMessenger::run()
    {
        while (!connect() && !threadShouldExit()) {
            for (int i = 0; i < 50 && !threadShouldExit(); ++i)
                sleep(100);
        }

        runImpl();
    }

    void WFSMessenger::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged,
                                                const juce::Identifier &property)
    {
        if (!connected) { return; }

        if (property == ananas::Utils::Identifiers::ModulesParamID) {
            if (auto *obj = treeWhosePropertyHasChanged[property].getDynamicObject()) {
                for (const auto &prop: obj->getProperties()) {
                    auto module{obj->getProperty(prop.name)};
                    auto id{module.getProperty(ananas::Utils::Identifiers::ModuleIDPropertyID, 0)};
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
        if (!connected) { return; }

        juce::OSCBundle bundle;
        DBG("Sending OSC: " << parameterID << " " << newValue);
        bundle.addElement(juce::OSCMessage{parameterID, newValue});
        send(bundle);
    }

    void WFSMessenger::runImpl() const
    {
        while (!threadShouldExit()) {
            // for (int i = 0; i < 10 && !threadShouldExit(); ++i)
                // wait(100);
        }
    }
} // ananas::WFS

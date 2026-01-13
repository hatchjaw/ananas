#include "WFSMessenger.h"
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
        const auto &propString{property.toString()};
        if (propString.contains("module")) {
            auto val{treeWhosePropertyHasChanged.getProperty(property).toString()};
            if (val.isNotEmpty()) {
                juce::OSCBundle bundle;
                DBG("Sending OSC: " << propString << " " << val);
                bundle.addElement(juce::OSCMessage{propString, val});
                send(bundle);
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

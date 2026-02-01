#ifndef PLUGINEDITOR_H
#define PLUGINEDITOR_H

#include "PluginProcessor.h"
#include "UI/LookAndFeel.h"
#include "UI/network/NetworkOverviewComponent.h"
#include "UI/wfs/WFSInterfaceComponent.h"

class PluginEditor final : public juce::AudioProcessorEditor,
                           public juce::ValueTree::Listener,
                           public juce::ApplicationCommandTarget
{
public:
    explicit PluginEditor(PluginProcessor &);

    ~PluginEditor() override;

    void paint(juce::Graphics &) override;

    void resized() override;

    void valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override;

    ApplicationCommandTarget *getNextCommandTarget() override;

    void getAllCommands(juce::Array<int> &commands) override;

    void getCommandInfo(juce::CommandID commandID, juce::ApplicationCommandInfo &result) override;

    bool perform(const InvocationInfo &info) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)

    PluginProcessor &getProcessor();

    [[nodiscard]] const PluginProcessor &getProcessor() const;

    juce::TooltipWindow tooltipWindow;
    ananas::LookAndFeel lookAndFeel;
    juce::TabbedComponent tabbedComponent{juce::TabbedButtonBar::TabsAtTop};
    ananas::NetworkOverviewComponent networkOverview;
    ananas::WFS::WFSInterfaceComponent wfsInterface;
    juce::ApplicationCommandManager commandManager;
};


#endif //PLUGINEDITOR_H

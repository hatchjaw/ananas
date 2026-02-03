#include "WFSInterfaceComponent.h"
#include <AnanasUtils.h>
#include <Server.h>

#include "../../Utils.h"

namespace ananas::WFS
{
    WFSInterfaceComponent::WFSInterfaceComponent(
        const uint numSources,
        const uint numModules,
        juce::AudioProcessorValueTreeState &apvts,
        juce::ValueTree &persistentTreeToListenTo
    ) : state(apvts),
        xyController(numSources, apvts),
        persistentTree(persistentTreeToListenTo)
    {
        addAndMakeVisible(xyController);

        addAndMakeVisible(speakerSpacingLabel);
        speakerSpacingLabel.setColour(juce::Label::textColourId, juce::Colours::black);
        speakerSpacingLabel.attachToComponent(&speakerSpacingSlider, false);
        speakerSpacingLabel.setText(Params::SpeakerSpacing.name, juce::dontSendNotification);
        speakerSpacingLabel.setJustificationType(juce::Justification::centredRight);

        addAndMakeVisible(speakerSpacingSlider);
        speakerSpacingSlider.setSliderStyle(juce::Slider::SliderStyle::IncDecButtons);
        speakerSpacingSlider.setNormalisableRange(Params::SpeakerSpacing.rangeDouble);
        speakerSpacingSlider.setValue(Params::SpeakerSpacing.defaultValue);
        speakerSpacingSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,
                                             false,
                                             speakerSpacingSlider.getTextBoxWidth(),
                                             25);
        speakerSpacingSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
        speakerSpacingSlider.setIncDecButtonsMode(juce::Slider::incDecButtonsDraggable_Vertical);

        speakerSpacingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            state,
            Params::SpeakerSpacing.id,
            speakerSpacingSlider
        );

        addAndMakeVisible(showModuleSelectorsButton);
        showModuleSelectorsButton.setButtonText(Params::ShowModuleSelectors.name);
        showModuleSelectorsButton.setColour(juce::ToggleButton::textColourId, juce::Colours::black);
        showModuleSelectorsButton.setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
        showModuleSelectorsButton.setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::black);

        showModuleSelectorsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            state,
            Params::ShowModuleSelectors.id,
            showModuleSelectorsButton
        );

        for (uint n{0}; n < numModules; ++n) {
            const auto m{modules.add(new ModuleComponent(n, persistentTree))};
            addAndMakeVisible(m);
            m->setBroughtToFrontOnMouseClick(true);
        }

        // Set initial module selector display state.
        parameterChanged(Params::ShowModuleSelectors.id, state.getRawParameterValue(Params::ShowModuleSelectors.id)->load());

        // Listen for changes to module selector display state.
        state.addParameterListener(Params::ShowModuleSelectors.id, this);

        // Listen to the persistent tree for module ID changes
        persistentTree.addListener(this);

        // Set initial moduleIDs from the persistent tree... todo: needs to be fixed
        updateModuleLists(persistentTree[ananas::Identifiers::ModulesParamID]);
    }

    WFSInterfaceComponent::~WFSInterfaceComponent()
    {
        state.removeParameterListener(Params::ShowModuleSelectors.id, this);
        persistentTree.removeListener(this);
        showModuleSelectorsButton.setLookAndFeel(nullptr);
    }

    void WFSInterfaceComponent::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    void WFSInterfaceComponent::resized()
    {
        auto bounds{getLocalBounds()};
        auto optionsRow{
            bounds.removeFromTop(Constants::UI::SpeakerSpacingSectionHeight)
            .reduced(6, 0)
        };
        speakerSpacingSlider.setBounds(optionsRow.removeFromRight(100).reduced(1, 3));
        speakerSpacingLabel.setBounds(optionsRow.removeFromRight(200));
        showModuleSelectorsButton.setBounds(optionsRow.removeFromLeft(300));
        bounds = bounds.reduced(10);
        xyController.setBounds(bounds);

        const auto moduleHeight{
            state.getRawParameterValue(Params::ShowModuleSelectors.id)->load() > 0.5 ? Constants::UI::ModuleHeight : Constants::UI::ModuleSpeakerHeight
        };

        const auto yZero{Constants::MaxYMetres * bounds.getHeight() / (Constants::MaxYMetres - Constants::MinYMetres)};
        bounds.removeFromTop(yZero - moduleHeight);

        juce::FlexBox flex;
        flex.flexDirection = juce::FlexBox::Direction::row;

        for (auto *m: modules) {
            flex.items.add(juce::FlexItem(*m)
                .withFlex(1.f) // Equal flex = equal width
                .withMaxHeight(moduleHeight));
        }

        flex.performLayout(bounds);

#ifdef SHOW_NO_NETWORK_OVERLAY
        OverlayableComponent::resized();
#endif
    }

    void WFSInterfaceComponent::updateModuleLists(const juce::var &var)
    {
        juce::StringArray ips;
        if (auto *obj = var.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                auto module{obj->getProperty(prop.name)};
                if (module.getProperty(ananas::Identifiers::ModuleIsConnectedPropertyID, false)) {
                    ips.add(prop.name.toString());
                }
            }
        }
        for (const auto &m: modules) {
            m->setAvailableModules(ips);
        }
    }

    void WFSInterfaceComponent::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
    {
        if (!isVisible()) return;

        if (property == ananas::Identifiers::ModulesParamID) {
            updateModuleLists(treeWhosePropertyHasChanged[property]);
        }
    }

    void WFSInterfaceComponent::parameterChanged(const juce::String &parameterID, const float newValue)
    {
        if (parameterID == Params::ShowModuleSelectors.id) {
            const auto show{newValue > .5f};
            for (auto *m: modules) {
                m->shouldShowModuleSelector(show);
            }
        }
        resized();
    }
} // ananas::WFS

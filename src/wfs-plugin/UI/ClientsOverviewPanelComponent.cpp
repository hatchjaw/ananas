#include "ClientsOverviewPanelComponent.h"

#include <AnanasUtils.h>
#include "../Utils.h"

namespace ananas
{
    ClientsOverviewPanelComponent::ClientsOverviewPanelComponent()
    {
        addAndMakeVisible(totalClientsLabel);
        addAndMakeVisible(totalClientsValue);
        addAndMakeVisible(presentationTimeIntervalLabel);
        addAndMakeVisible(presentationTimeIntervalValue);

        // TODO: use LookAndFeel instead
        totalClientsLabel.setColour(juce::Label::textColourId, juce::Colours::black);
        totalClientsValue.setColour(juce::Label::textColourId, juce::Colours::black);
        presentationTimeIntervalLabel.setColour(juce::Label::textColourId, juce::Colours::black);
        presentationTimeIntervalValue.setColour(juce::Label::textColourId, juce::Colours::black);

        totalClientsLabel.setText(WFS::Strings::TotalClientsLabel, juce::dontSendNotification);
        presentationTimeIntervalLabel.setText(WFS::Strings::PresentationTimeIntervalLabel, juce::dontSendNotification);
    }

    void ClientsOverviewPanelComponent::update(const juce::var &var)
    {
        if (!isVisible()) return;

        if (auto *obj = var.getDynamicObject()) {
            const auto &props{obj->getProperties()};
            totalClientsValue.setText(juce::String{props.size()}, juce::dontSendNotification);

            juce::int32 minOffsetTime{INT32_MAX}, maxOffsetTime{INT32_MIN};
            for (const auto &prop: props) {
                if (const auto *client = prop.value.getDynamicObject()) {
                    const juce::int32 presentationTimeOffset{
                                client->getProperty(Identifiers::ClientPresentationTimeOffsetNsPropertyID)
                            },
                            audioPTPOffset{
                                client->getProperty(Identifiers::ClientAudioPTPOffsetPropertyID)
                            };
                    const auto offsetTime{presentationTimeOffset + audioPTPOffset};
                    if (offsetTime < minOffsetTime) {
                        minOffsetTime = offsetTime;
                    }
                    if (offsetTime > maxOffsetTime) {
                        maxOffsetTime = offsetTime;
                    }
                }
            }
            presentationTimeIntervalValue.setText(juce::String{maxOffsetTime - minOffsetTime} + " ns", juce::dontSendNotification);
        }
    }

    void ClientsOverviewPanelComponent::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    void ClientsOverviewPanelComponent::resized()
    {
        auto bounds = getLocalBounds().reduced(10);

        // Split into left and right halves
        const int halfWidth = bounds.getWidth() / 2;

        // Left stat
        const auto leftBounds = bounds.removeFromLeft(halfWidth);
        juce::FlexBox leftFlex;
        leftFlex.flexDirection = juce::FlexBox::Direction::row;
        leftFlex.items.add(juce::FlexItem(totalClientsLabel).withWidth(90));
        leftFlex.items.add(juce::FlexItem(totalClientsValue).withFlex(1));
        leftFlex.performLayout(leftBounds);

        // Right stat
        juce::FlexBox rightFlex;
        rightFlex.flexDirection = juce::FlexBox::Direction::row;
        rightFlex.items.add(juce::FlexItem(presentationTimeIntervalLabel).withWidth(200));
        rightFlex.items.add(juce::FlexItem(presentationTimeIntervalValue).withFlex(1));
        rightFlex.performLayout(bounds);
    }
}

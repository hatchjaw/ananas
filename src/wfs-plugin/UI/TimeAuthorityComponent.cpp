#include "TimeAuthorityComponent.h"

#include <AnanasUtils.h>

ananas::TimeAuthorityComponent::TimeAuthorityComponent()
{
    addAndMakeVisible(title);
    addAndMakeVisible(table);
}

void ananas::TimeAuthorityComponent::update(juce::var var)
{
    table.update(var);
}

void ananas::TimeAuthorityComponent::paint (juce::Graphics& g)
{

}

void ananas::TimeAuthorityComponent::resized()
{

}

void ananas::TimeAuthorityComponent::TimeAuthorityTable::update(const juce::var &var)
{
    const auto object = var.getDynamicObject();
    row.ip = object->getProperty(Identifiers::AuthorityIpPropertyID);
    row.serialNumber = object->getProperty(Identifiers::AuthoritySerialNumberPropertyID);
}

int ananas::TimeAuthorityComponent::TimeAuthorityTable::getNumRows()
{
    return 1;
}

void ananas::TimeAuthorityComponent::TimeAuthorityTable::paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
{
}

void ananas::TimeAuthorityComponent::TimeAuthorityTable::paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    const auto &[ip, serialNumber, feedbackAccumulator] = row;
    juce::String text;
    juce::Justification justification{juce::Justification::centredLeft};

    g.setColour(juce::Colours::black);
    g.setFont(14.0f);
    g.drawText(text, 2, 0, width - 4, height, justification, true);
}

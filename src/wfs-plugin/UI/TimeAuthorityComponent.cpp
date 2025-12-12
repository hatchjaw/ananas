#include "TimeAuthorityComponent.h"

#include <AnanasUtils.h>

ananas::TimeAuthorityComponent::TimeAuthorityComponent(juce::ValueTree &treeToListenTo)
    : tree(treeToListenTo)
{
    addAndMakeVisible(title);
    addAndMakeVisible(authorityTable);

    title.setColour(juce::Label::textColourId, juce::Colours::black);
    title.setFont(juce::Font(juce::FontOptions(15.f, juce::Font::bold)));
    title.setJustificationType(juce::Justification::centredLeft);
    title.setText(WFS::Strings::TimeAuthoritySectionTitle, juce::dontSendNotification);

    tree.addListener(this);
}

ananas::TimeAuthorityComponent::~TimeAuthorityComponent()
{
    tree.removeListener(this);
}

void ananas::TimeAuthorityComponent::update(juce::var var)
{
    authorityTable.update(var);
}

void ananas::TimeAuthorityComponent::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::transparentWhite);
}

void ananas::TimeAuthorityComponent::resized()
{
    auto bounds{getLocalBounds()};
    title.setBounds(
        bounds.removeFromTop(WFS::Constants::NetworkSectionTitleHeight)
        .reduced(6, 0)
    );
    authorityTable.setBounds(bounds);
}

void ananas::TimeAuthorityComponent::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
{
    if (!isVisible()) return;

    if (property == Identifiers::TimeAuthorityParamID) {
        update(treeWhosePropertyHasChanged[property]);
        handleAsyncUpdate();
    }
}

void ananas::TimeAuthorityComponent::handleAsyncUpdate()
{
    repaint();
}

//==============================================================================

ananas::TimeAuthorityComponent::TimeAuthorityTable::TimeAuthorityTable()
{
    addAndMakeVisible(table);

    addColumn(WFS::TableColumns::AuthorityTableIpAddress);
    addColumn(WFS::TableColumns::AuthorityTableSerialNumber);
    addColumn(WFS::TableColumns::AuthorityTableFeedbackAccumulator);
    addColumn(WFS::TableColumns::AuthorityTableSamplingRate);

    table.setModel(this);
    table.setColour(juce::ListBox::outlineColourId, juce::Colours::black);
    table.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentWhite);
    table.setOutlineThickness(1);
}

void ananas::TimeAuthorityComponent::TimeAuthorityTable::update(const juce::var &var)
{
    if (!isVisible()) return;

    const auto object{var.getDynamicObject()};

    row.ip = object->getProperty(Identifiers::AuthorityIpPropertyID);
    row.serialNumber = object->getProperty(Identifiers::AuthoritySerialNumberPropertyID);

    const auto feedbackAccumulator{static_cast<int>(object->getProperty(Identifiers::AuthorityFeedbackAccumulatorPropertyID))},
            feedbackAccumulatorDiff{feedbackAccumulator - Constants::AuthorityInitialUSBFeedbackAccumulator};

    row.feedbackAccumulator = juce::String{feedbackAccumulator} + (feedbackAccumulatorDiff >= 0 ? " (+" : " (") + juce::String{feedbackAccumulatorDiff} + ")";

    row.samplingRate = WFS::Utils::formatWithThousandsSeparator(1000 * static_cast<float>(feedbackAccumulator >> 12) / static_cast<float>(1 << 13)) +
                       " (0x" + juce::String{feedbackAccumulator >> 28} + "." +
                       juce::String::toHexString(feedbackAccumulator >> 12 & 0xFFFF).paddedLeft('0', 4) + ")";

    table.updateContent();
    repaint();
}

int ananas::TimeAuthorityComponent::TimeAuthorityTable::getNumRows()
{
    return 1;
}

void ananas::TimeAuthorityComponent::TimeAuthorityTable::paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
{
    juce::ignoreUnused(width, height, rowNumber, rowIsSelected);

    g.fillAll(juce::Colours::white);
}

void ananas::TimeAuthorityComponent::TimeAuthorityTable::paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    juce::ignoreUnused(rowIsSelected, rowNumber);

    const auto &[ip, serialNumber, feedbackAccumulator, samplingRate] = row;
    juce::String text;
    juce::Justification justification{juce::Justification::centredLeft};

    switch (columnId) {
        case 1: text = ip;
            justification = WFS::TableColumns::AuthorityTableIpAddress.justification;
            break;
        case 2: text = serialNumber;
            justification = WFS::TableColumns::AuthorityTableSerialNumber.justification;
            break;
        case 3: text = feedbackAccumulator;
            justification = WFS::TableColumns::AuthorityTableFeedbackAccumulator.justification;
            break;
        case 4: text = samplingRate;
            justification = WFS::TableColumns::AuthorityTableSamplingRate.justification;
            break;
        default: break;
    }

    g.setColour(juce::Colours::black);
    g.setFont(14.0f);
    g.drawText(text, 2, 0, width - 4, height, justification, true);
}

void ananas::TimeAuthorityComponent::TimeAuthorityTable::resized()
{
    table.setBounds(getLocalBounds().reduced(10));
}

void ananas::TimeAuthorityComponent::TimeAuthorityTable::addColumn(const WFS::TableColumns::ColumnHeader &h) const
{
    table.getHeader().addColumn(h.label, h.id, h.width, h.minWidth, h.maxWidth, h.flags);
}

#include "ClientsOverviewComponent.h"
#include <AnanasUtils.h>

namespace ananas::UI
{
    ClientsOverviewComponent::ClientsOverviewComponent(juce::ValueTree &treeToListenTo)
        : tree(treeToListenTo)
    {
        addAndMakeVisible(title);
        addAndMakeVisible(rebootAllClientsButton);
        addAndMakeVisible(overviewPanel);
        addAndMakeVisible(clientTable);

        title.setColour(juce::Label::textColourId, juce::Colours::black);
        title.setFont(juce::Font(juce::FontOptions(15.f, juce::Font::bold)));
        title.setJustificationType(juce::Justification::centredLeft);
        title.setText(Strings::ClientsSectionTitle, juce::dontSendNotification);

        rebootAllClientsButton.setButtonText(Strings::RebootAllClientsButtonText);
        rebootAllClientsButton.setTooltip(Strings::RebootAllClientsButtonTooltip);
        rebootAllClientsButton.onClick = [this]
        {
            triggerClientReboot();
        };

        tree.addListener(this);
    }

    ClientsOverviewComponent::~ClientsOverviewComponent()
    {
        tree.removeListener(this);
    }

    void ClientsOverviewComponent::update(const juce::var &var)
    {
        if (!isVisible()) return;

        clientTable.update(var);
        overviewPanel.update(var);
    }

    void ClientsOverviewComponent::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    void ClientsOverviewComponent::resized()
    {
        auto bounds{getLocalBounds()};
        auto titleRow{
            bounds.removeFromTop(Dimensions::NetworkSectionTitleHeight)
            .reduced(6, 0)
        };
        title.setBounds(titleRow.removeFromLeft(85));
        rebootAllClientsButton.setBounds(titleRow.removeFromLeft(100).reduced(8));
        overviewPanel.setBounds(bounds.removeFromTop(35));
        clientTable.setBounds(bounds);
    }

    void ClientsOverviewComponent::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
    {
        if (!isVisible()) return;

        if (property == ananas::Identifiers::ConnectedClientsParamID) {
            update(treeWhosePropertyHasChanged[property]);
            handleAsyncUpdate();
        }
    }

    void ClientsOverviewComponent::handleAsyncUpdate()
    {
        repaint();
    }

    void ClientsOverviewComponent::triggerClientReboot() const
    {
        tree.setProperty(ananas::Identifiers::ClientsShouldRebootParamID, true, nullptr);
    }

    //==========================================================================

    ClientsOverviewComponent::OverviewPanel::OverviewPanel()
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

        totalClientsLabel.setText(Strings::TotalClientsLabel, juce::dontSendNotification);
        presentationTimeIntervalLabel.setText(Strings::PresentationTimeIntervalLabel, juce::dontSendNotification);
    }

    void ClientsOverviewComponent::OverviewPanel::update(const juce::var &var)
    {
        if (!isVisible()) return;

        if (auto *obj = var.getDynamicObject()) {
            const auto &props{obj->getProperties()};
            totalClientsValue.setText(juce::String{props.size()}, juce::dontSendNotification);

            juce::int32 minOffsetTime{INT32_MAX}, maxOffsetTime{INT32_MIN};
            for (const auto &prop: props) {
                if (const auto *client = prop.value.getDynamicObject()) {
                    const juce::int32 presentationTimeOffset{
                                client->getProperty(ananas::Identifiers::ClientPresentationTimeOffsetNsPropertyID)
                            },
                            audioPTPOffset{
                                client->getProperty(ananas::Identifiers::ClientAudioPTPOffsetPropertyID)
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

    void ClientsOverviewComponent::OverviewPanel::paint(juce::Graphics &g)
    {
        g.fillAll(juce::Colours::transparentWhite);
    }

    void ClientsOverviewComponent::OverviewPanel::resized()
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

    //==========================================================================

    ClientsOverviewComponent::ClientTable::ClientTable()
    {
        addAndMakeVisible(table);

        addColumn(TableColumns::ClientTableIpAddress);
        addColumn(TableColumns::ClientTableSerialNumber);
        addColumn(TableColumns::ClientTablePTPLock);
        addColumn(TableColumns::ClientTablePresentationTimeOffset);
        addColumn(TableColumns::ClientTableBufferFillPercent);
        addColumn(TableColumns::ClientTableSamplingRate);
        addColumn(TableColumns::ClientTablePercentCPU);
        addColumn(TableColumns::ClientTableModuleID);

        table.getHeader().setLookAndFeel(&lookAndFeel);

        table.setModel(this);
        table.setColour(juce::ListBox::outlineColourId, juce::Colours::black);
        table.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentWhite);
        table.setOutlineThickness(1);
    }

    ClientsOverviewComponent::ClientTable::~ClientTable()
    {
        table.getHeader().setLookAndFeel(nullptr);
    }

    void ClientsOverviewComponent::ClientTable::update(const juce::var &clientInfo)
    {
        if (!isVisible()) return;

        rows.clear();

        if (auto *obj = clientInfo.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                Row row;
                row.ip = prop.name.toString();

                if (const auto *client = prop.value.getDynamicObject()) {
                    row.serialNumber = client->getProperty(ananas::Identifiers::ClientSerialNumberPropertyID).toString();
                    row.ptpLock = client->getProperty(ananas::Identifiers::ClientPTPLockPropertyID);
                    row.presentationTimeOffsetNs = client->getProperty(ananas::Identifiers::ClientPresentationTimeOffsetNsPropertyID);
                    row.presentationTimeOffsetFrame = client->getProperty(ananas::Identifiers::ClientPresentationTimeOffsetFramePropertyID);
                    row.audioPTPOffsetNs = client->getProperty(ananas::Identifiers::ClientAudioPTPOffsetPropertyID);
                    row.bufferFillPercent = client->getProperty(ananas::Identifiers::ClientBufferFillPercentPropertyID);
                    row.samplingRate = client->getProperty(ananas::Identifiers::ClientSamplingRatePropertyID);
                    row.percentCPU = client->getProperty(ananas::Identifiers::ClientPercentCPUPropertyID);
                    row.moduleID = client->getProperty(ananas::Identifiers::ClientModuleIDPropertyID);
                }

                rows.add(row);
            }
        }

        table.updateContent();
        repaint();
    }

    int ClientsOverviewComponent::ClientTable::getNumRows()
    {
        return rows.size();
    }

    void ClientsOverviewComponent::ClientTable::paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
    {
        juce::ignoreUnused(width, height);

        if (rowIsSelected)
            g.fillAll(juce::Colours::lightblue);
        else if (rowNumber % 2 == 0)
            g.fillAll(juce::Colour(0xffeeeeee));
        else
            g.fillAll(juce::Colours::white);
    }

    void ClientsOverviewComponent::ClientTable::paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
    {
        juce::ignoreUnused(rowIsSelected);

        if (rowNumber < rows.size()) {
            const auto &[
                ip,
                serialNumber,
                ptpLock,
                offsetTime,
                offsetFrame,
                audioPTPOffset,
                bufferFillPercent,
                samplingRate,
                percentCPU,
                moduleID
            ] = rows[rowNumber];
            juce::String text;
            juce::Justification justification{juce::Justification::centredLeft};

            switch (columnId) {
                case 1: text = ip;
                    justification = TableColumns::ClientTableIpAddress.justification;
                    break;
                case 2: text = serialNumber;
                    justification = TableColumns::ClientTableSerialNumber.justification;
                    break;
                case 3: text = ptpLock ? "Yes" : "No";
                    justification = TableColumns::ClientTablePTPLock.justification;
                    g.setColour(ptpLock ? juce::Colours::lightseagreen : juce::Colours::palevioletred);
                    g.fillRect(2, 2, width - 4, height - 4);
                    break;
                case 4: text = Strings::formatWithThousandsSeparator(offsetTime + audioPTPOffset) +
                               " (" + juce::String(offsetFrame) +
                               (offsetFrame == 1 ? " frame)" : " frames)");
                    justification = TableColumns::ClientTablePresentationTimeOffset.justification;
                    break;
                case 5: text = juce::String(bufferFillPercent) + " %";
                    justification = TableColumns::ClientTableBufferFillPercent.justification;
                    g.setColour(bufferFillPercent > 80 || bufferFillPercent < 20 ? juce::Colours::palevioletred : juce::Colours::lightseagreen);
                    g.fillRect(2, 2, static_cast<int>((width - 4) * (bufferFillPercent / 100.f)), height - 4);
                    break;
                case 6: text = Strings::formatWithThousandsSeparator(samplingRate, 6);
                    justification = TableColumns::ClientTableSamplingRate.justification;
                    break;
                case 7: text = juce::String(percentCPU, 3);
                    justification = TableColumns::ClientTablePercentCPU.justification;
                    break;
                case 8: text = juce::String{moduleID};
                    justification = TableColumns::ClientTableModuleID.justification;
                default: break;
            }

            g.setColour(juce::Colours::black);
            g.setFont(14.0f);
            g.drawText(text, 2, 0, width - 4, height, justification, true);
        }
    }

    void ClientsOverviewComponent::ClientTable::resized()
    {
        table.setBounds(getLocalBounds().reduced(10));
    }

    //==========================================================================

    void ClientsOverviewComponent::ClientTable::LookAndFeel::drawTableHeaderColumn(juce::Graphics &g, juce::TableHeaderComponent &header,
                                                                                   const juce::String &columnName, int columnId, int width, int height,
                                                                                   bool isMouseOver,
                                                                                   bool isMouseDown, int columnFlags)
    {
        auto highlightColour = header.findColour(juce::TableHeaderComponent::highlightColourId);

        if (isMouseDown)
            g.fillAll(highlightColour);
        else if (isMouseOver)
            g.fillAll(highlightColour.withMultipliedAlpha(0.625f));

        juce::Rectangle<int> area(width, height);
        area.reduce(4, 0);

        if ((columnFlags & (juce::TableHeaderComponent::sortedForwards | juce::TableHeaderComponent::sortedBackwards)) != 0) {
            juce::Path sortArrow;
            sortArrow.addTriangle(0.0f, 0.0f,
                                  0.5f, (columnFlags & juce::TableHeaderComponent::sortedForwards) != 0 ? -0.8f : 0.8f,
                                  1.0f, 0.0f);

            g.setColour(juce::Colour(0x99000000));
            g.fillPath(sortArrow, sortArrow.getTransformToScaleToFit(area.removeFromRight(height / 2).reduced(2).toFloat(), true));
        }

        g.setColour(header.findColour(juce::TableHeaderComponent::textColourId));
        g.setFont(withDefaultMetrics(juce::FontOptions((float) height * 0.5f, juce::Font::bold)));
        auto justification{juce::Justification::centredLeft};
        switch (columnId) {
            case 1: justification = TableColumns::ClientTableIpAddress.justification;
                break;
            case 2: justification = TableColumns::ClientTableSerialNumber.justification;
                break;
            case 3: justification = TableColumns::ClientTablePTPLock.justification;
                break;
            case 4: justification = TableColumns::ClientTablePresentationTimeOffset.justification;
                break;
            case 5: justification = TableColumns::ClientTableBufferFillPercent.justification;
                break;
            case 6: justification = TableColumns::ClientTableSamplingRate.justification;
                break;
            case 7: justification = TableColumns::ClientTablePercentCPU.justification;
                break;
            case 8: justification = TableColumns::ClientTableModuleID.justification;
            default:
                break;
        }
        g.drawFittedText(columnName, area, justification, 1);
    }
}

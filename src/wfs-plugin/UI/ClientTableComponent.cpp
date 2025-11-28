#include "ClientTableComponent.h"

#include <AnanasUtils.h>
#include "../Utils.h"

namespace ananas
{
    ClientTableComponent::ClientTableComponent()
    {
        addAndMakeVisible(table);

        addColumn(WFS::TableColumns::ClientTableIpAddress);
        addColumn(WFS::TableColumns::ClientTableSerialNumber);
        addColumn(WFS::TableColumns::ClientTablePTPLock);
        addColumn(WFS::TableColumns::ClientTablePresentationTimeOffset);
        addColumn(WFS::TableColumns::ClientTableBufferFillPercent);
        addColumn(WFS::TableColumns::ClientTableSamplingRate);
        addColumn(WFS::TableColumns::ClientTablePercentCPU);

        table.getHeader().setLookAndFeel(&lookAndFeel);

        table.setModel(this);
        table.setColour(juce::ListBox::outlineColourId, juce::Colours::black);
        table.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentWhite);
        table.setOutlineThickness(1);
    }

    ClientTableComponent::~ClientTableComponent()
    {
        table.getHeader().setLookAndFeel(nullptr);
    }

    void ClientTableComponent::update(const juce::var &clientInfo)
    {
        if (!isVisible()) return;

        rows.clear();

        if (auto *obj = clientInfo.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                Row row;
                row.ip = prop.name.toString();

                if (const auto *client = prop.value.getDynamicObject()) {
                    row.serialNumber = client->getProperty(Identifiers::ClientSerialNumberPropertyID).toString();
                    row.ptpLock = client->getProperty(Identifiers::ClientPTPLockPropertyID);
                    row.presentationTimeOffsetNs = client->getProperty(Identifiers::ClientPresentationTimeOffsetNsPropertyID);
                    row.presentationTimeOffsetFrame = client->getProperty(Identifiers::ClientPresentationTimeOffsetFramePropertyID);
                    row.audioPTPOffsetNs = client->getProperty(Identifiers::ClientAudioPTPOffsetPropertyID);
                    row.bufferFillPercent = client->getProperty(Identifiers::ClientBufferFillPercentPropertyID);
                    row.samplingRate = client->getProperty(Identifiers::ClientSamplingRatePropertyID);
                    row.percentCPU = client->getProperty(Identifiers::ClientPercentCPUPropertyID);
                }

                rows.add(row);
            }
        }

        table.updateContent();
        repaint();
    }

    int ClientTableComponent::getNumRows()
    {
        return rows.size();
    }

    void ClientTableComponent::paintRowBackground(juce::Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
    {
        juce::ignoreUnused(width, height);

        if (rowIsSelected)
            g.fillAll(juce::Colours::lightblue);
        else if (rowNumber % 2 == 0)
            g.fillAll(juce::Colour(0xffeeeeee));
        else
            g.fillAll(juce::Colours::white);
    }

    void ClientTableComponent::paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
    {
        juce::ignoreUnused(rowIsSelected);

        if (rowNumber < rows.size()) {
            const auto &[ip, serialNumber, ptpLock, offsetTime, offsetFrame, audioPTPOffset, bufferFillPercent, samplingRate, percentCPU] = rows[rowNumber];
            juce::String text;
            juce::Justification justification{juce::Justification::centredLeft};

            switch (columnId) {
                case 1: text = ip;
                    justification = WFS::TableColumns::ClientTableIpAddress.justification;
                    break;
                case 2: text = serialNumber;
                    justification = WFS::TableColumns::ClientTableSerialNumber.justification;
                    break;
                case 3: text = ptpLock ? "Yes" : "No";
                    justification = WFS::TableColumns::ClientTablePTPLock.justification;
                    g.setColour(ptpLock ? juce::Colours::lightseagreen : juce::Colours::palevioletred);
                    g.fillRect(2, 2, width - 4, height - 4);
                    break;
                case 4: text = WFS::Utils::formatWithThousandsSeparator(offsetTime + audioPTPOffset) +
                               " (" + juce::String(offsetFrame) +
                               (offsetFrame == 1 ? " frame)" : " frames)");
                    justification = WFS::TableColumns::ClientTablePresentationTimeOffset.justification;
                    break;
                case 5: text = juce::String(bufferFillPercent) + " %";
                    justification = WFS::TableColumns::ClientTableBufferFillPercent.justification;
                    g.setColour(bufferFillPercent > 80 || bufferFillPercent < 20 ? juce::Colours::palevioletred : juce::Colours::lightseagreen);
                    g.fillRect(2, 2, static_cast<int>((width - 4) * (bufferFillPercent / 100.f)), height - 4);
                    break;
                case 6: text = WFS::Utils::formatWithThousandsSeparator(samplingRate, 6);
                    justification = WFS::TableColumns::ClientTableSamplingRate.justification;
                    break;
                case 7: text = juce::String(percentCPU, 3);
                    justification = WFS::TableColumns::ClientTablePercentCPU.justification;
                    break;
                default: break;
            }

            g.setColour(juce::Colours::black);
            g.setFont(14.0f);
            g.drawText(text, 2, 0, width - 4, height, justification, true);
        }
    }

    void ClientTableComponent::resized()
    {
        table.setBounds(getLocalBounds().reduced(10));
    }

    void ClientTableComponent::addColumn(const WFS::TableColumns::ColumnHeader &h) const
    {
        table.getHeader().addColumn(h.label, h.id, h.width, h.minWidth, h.maxWidth, h.flags);
    }

    //==========================================================================

    void ClientTableComponent::LookAndFeel::drawTableHeaderColumn(juce::Graphics &g, juce::TableHeaderComponent &header,
                                                                  const juce::String &columnName, int columnId, int width, int height, bool isMouseOver,
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
            case 1: justification = WFS::TableColumns::ClientTableIpAddress.justification;
                break;
            case 2: justification = WFS::TableColumns::ClientTableSerialNumber.justification;
                break;
            case 3: justification = WFS::TableColumns::ClientTablePTPLock.justification;
                break;
            case 4: justification = WFS::TableColumns::ClientTablePresentationTimeOffset.justification;
                break;
            case 5: justification = WFS::TableColumns::ClientTableBufferFillPercent.justification;
                break;
            case 6: justification = WFS::TableColumns::ClientTableSamplingRate.justification;
                break;
            case 7: justification = WFS::TableColumns::ClientTablePercentCPU.justification;
                break;
            default:
                break;
        }
        g.drawFittedText(columnName, area, justification, 1);
    }
} // ananas

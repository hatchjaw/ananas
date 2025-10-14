#include "ClientTableComponent.h"

#include <Utils.h>
#include "../Utils.h"

namespace ananas
{
    ClientTableComponent::ClientTableComponent()
    {
        addAndMakeVisible(table);

        addColumn(WFS::Tables::ClientTableIpAddress);
        addColumn(WFS::Tables::ClientTableSerialNumber);
        addColumn(WFS::Tables::ClientTablePresentationTimeOffset);
        addColumn(WFS::Tables::ClientTableBufferFillPercent);
        addColumn(WFS::Tables::ClientTableSamplingRate);
        addColumn(WFS::Tables::ClientTablePercentCPU);

        table.setModel(this);
        table.setColour(juce::ListBox::outlineColourId, juce::Colours::black);
        table.setOutlineThickness(1);
    }

    void ClientTableComponent::update(const juce::var &clientInfo)
    {
        rows.clear();

        if (auto *obj = clientInfo.getDynamicObject()) {
            for (const auto &prop: obj->getProperties()) {
                Row row;
                row.ip = prop.name.toString();

                if (const auto *client = prop.value.getDynamicObject()) {
                    row.serialNumber = client->getProperty(Identifiers::ClientSerialNumberPropertyID).toString();
                    row.offsetTime = client->getProperty(Identifiers::ClientPtpOffsetNsPropertyID);
                    row.offsetFrame = client->getProperty(Identifiers::ClientPtpOffsetFramePropertyID);
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
        if (rowIsSelected)
            g.fillAll(juce::Colours::lightblue);
        else //if (rowNumber % 2 == 0)
            g.fillAll(juce::Colour(0xffeeeeee));
    }

    void ClientTableComponent::paintCell(juce::Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
    {
        g.setColour(juce::Colours::black);
        g.setFont(14.0f);

        if (rowNumber < rows.size()) {
            const auto &client = rows[rowNumber];
            juce::String text;
            juce::Justification justification{juce::Justification::centredLeft};

            switch (columnId) {
                case 1: text = client.ip;
                    break;
                case 2: text = client.serialNumber;
                    justification = juce::Justification::centred;
                    break;
                case 3: text = WFS::Utils::formatWithThousandsSeparator(client.offsetTime) + "  (" + juce::String(client.offsetFrame) + " frames)";
                    justification = juce::Justification::centredRight;
                    break;
                case 4: text = juce::String(client.bufferFillPercent);
                    justification = juce::Justification::centredRight;
                    break;
                case 5: text = WFS::Utils::formatWithThousandsSeparator(client.samplingRate, 6);
                    justification = juce::Justification::centredRight;
                    break;
                case 6: text = juce::String(client.percentCPU, 3);
                    justification = juce::Justification::centredRight;
                    break;
                default: break;
            }

            g.drawText(text, 2, 0, width - 4, height, justification, true);
        }
    }

    void ClientTableComponent::resized()
    {
        table.setBounds(getLocalBounds());
    }

    void ClientTableComponent::addColumn(const WFS::Tables::ColumnHeader &h) const
    {
        table.getHeader().addColumn(h.label, h.id, h.width, h.minWidth, h.maxWidth, h.flags);
    }
} // ananas

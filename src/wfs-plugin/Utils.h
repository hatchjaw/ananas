#ifndef WFSUTILS_H
#define WFSUTILS_H

#include <juce_core/juce_core.h>

#ifndef MAX_CHANNELS_TO_SEND
#define MAX_CHANNELS_TO_SEND 16
#endif

namespace ananas::WFS
{
    class Constants
    {
    public:
        constexpr static uint8_t MaxChannelsToSend{MAX_CHANNELS_TO_SEND};
    };

    class Params
    {
    public:
        struct Param
        {
            juce::StringRef id;
            juce::StringRef name;
        };

        inline static const Param SpeakerSpacing{"/spacing", "Speaker Spacing (m)"};
    };

    class TableColumns
    {
    public:
        struct ColumnHeader
        {
            int id;
            juce::StringRef label;
            int width;
            int minWidth{30};
            int maxWidth{-1};
            int flags{juce::TableHeaderComponent::defaultFlags};
            juce::Justification::Flags justification{juce::Justification::centredLeft};
        };

        inline static const ColumnHeader ClientTableIpAddress{
            1, "IP address", 150, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centredLeft
        };
        inline static const ColumnHeader ClientTableSerialNumber{
            2, "Serial number", 100, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centred
        };
        inline static const ColumnHeader ClientTablePTPLock{
            3, "PTP lock", 75, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centred
        };
        inline static const ColumnHeader ClientTablePresentationTimeOffset{
            4, "Presentation time offset (ns)", 200, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centredRight
        };
        inline static const ColumnHeader ClientTableBufferFillPercent{
            5, "Buffer fill level", 200, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centred
        };
        inline static const ColumnHeader ClientTableSamplingRate{
            6, "Reported sampling rate (Hz)", 200, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centred
        };
        inline static const ColumnHeader ClientTablePercentCPU{
            7, "CPU %", 75, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centredRight
        };
    };

    class Strings
    {
    public:
        inline static const juce::StringRef InputLabel{"Input #"};
        inline static const juce::StringRef OutputLabel{"Output #"};

        inline static const juce::StringRef NetworkTabName{"Network"};
        inline static const juce::StringRef WfsTabName{"WFS"};

        inline static const juce::StringRef TotalClientsLabel{"Total clients: "};
        inline static const juce::StringRef PresentationTimeIntervalLabel{"Approx. group asynchronicity: "};
        inline static const juce::StringRef ClientTableColumnNameIpAddress{"IP address"};
        inline static const juce::StringRef ClientTableColumnNameSerialNumber{"Serial number"};
        inline static const juce::StringRef ClientTableColumnNamePresentationTimeOffset{"Presentation time offset (ns)"};
        inline static const juce::StringRef ClientTableColumnNameBufferFillPercent{"Buffer fill level %"};
        inline static const juce::StringRef ClientTableColumnNameSamplingRate{"Reported sampling rate (Hz)"};
        inline static const juce::StringRef ClientTableColumnNamePercentCPU{"CPU %"};
    };

    class Identifiers
    {
    public:
        inline static const juce::Identifier StaticTreeType{"WfsParameters"};
        inline static const juce::Identifier DynamicTreeType{"ModuleParameters"};
    };

    class Utils
    {
    public:
        static juce::String formatWithThousandsSeparator(const int value)
        {
            std::stringstream ss;
            ss.imbue(std::locale("en_GB.UTF-8")); // Use system locale
            ss << value;
            return ss.str();
        }

        static juce::String formatWithThousandsSeparator(const float value, const int decimals = 3)
        {
            std::stringstream ss;
            ss.imbue(std::locale("en_GB.UTF-8")); // Use system locale
            ss << std::fixed << std::setprecision(decimals) << value;
            return ss.str();
        }
    };
}


#endif //WFSUTILS_H

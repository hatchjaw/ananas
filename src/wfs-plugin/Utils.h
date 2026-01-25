#ifndef WFSUTILS_H
#define WFSUTILS_H

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#ifndef MAX_CHANNELS_TO_SEND
#define MAX_CHANNELS_TO_SEND 16
#endif

#ifndef NUM_MODULES
#define NUM_MODULES 8
#endif

namespace ananas::WFS
{
    class Constants
    {
    public:
        constexpr static uint MaxChannelsToSend{MAX_CHANNELS_TO_SEND};
        constexpr static uint NumModules{NUM_MODULES};

        inline const static juce::StringRef LocalInterfaceIP{"192.168.10.10"};
        constexpr static int WFSMessengerSocketLocalPort{49160};
        constexpr static int WFSMessengerSocketRemotePort{49160};
        inline const static juce::StringRef WFSControlMulticastIP{"224.4.224.10"};

        class UI
        {
        public:
            constexpr static int UiWidth{1200};
            constexpr static int UiHeight{900};

            constexpr static int SwitchesSectionHeight{175};
            constexpr static int TimeAuthoritySectionHeight{112};
            constexpr static int NetworkSectionTitleHeight{40};

            constexpr static int SpeakerSpacingSectionHeight{50};
            constexpr static float NodeDiameter{40.f};
            constexpr static float NodeHalfDiameter{NodeDiameter / 2.f};

            constexpr static int ModuleSelectorHeight{30};
        };
    };

    enum class SourcePositionAxis : char
    {
        X = 'x',
        Y = 'y'
    };

    class Params
    {
    public:
        struct Param
        {
            juce::StringRef id;
            juce::StringRef name;
            juce::NormalisableRange<float> range;
            juce::NormalisableRange<double> rangeDouble;
            float defaultValue{0.f};
        };

        inline static const Param SpeakerSpacing{
            "/spacing",
            "Speaker Spacing (m)",
            {.05f, .3f, .001f},
            {.05, .3, .001},
            .2
        };

        constexpr static float SourcePositionDefaultX{.5f};
        constexpr static float SourcePositionDefaultY{.5f};
        inline static const juce::NormalisableRange<float> SourcePositionRange{-1.f, 1.f, 1e-6f};

        static juce::String getSourcePositionParamID(const uint index, SourcePositionAxis axis)
        {
            return "/source/" + juce::String{index} + "/" + static_cast<char>(axis);
        }

        static juce::String getSourcePositionParamName(const uint index, SourcePositionAxis axis)
        {
            return "Source " + juce::String{index + 1} + " " + static_cast<char>(axis);
        }

        static float getSourcePositionDefaultX(const uint sourceIndex)
        {
            return -1.f + (2.f * (static_cast<float>(sourceIndex) + SourcePositionDefaultX) / Constants::MaxChannelsToSend);
        }

        static juce::String getModuleIndexParamID(const int index)
        {
            return "/module/" + juce::String{index};
        }
    };

    class TableColumns
    {
    public:
        struct ColumnHeader
        {
            int id{};
            juce::StringRef label;
            int width{};
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
            2, "Serial number", 125, 30, -1,
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
        inline static const ColumnHeader ClientTableModuleID{
            8, "Module ID", 75, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centred
        };

        inline static const ColumnHeader AuthorityTableIpAddress{
            1, "IP address", 150, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centredLeft
        };
        inline static const ColumnHeader AuthorityTableSerialNumber{
            2, "Serial number", 125, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centred
        };
        inline static const ColumnHeader AuthorityTableFeedbackAccumulator{
            3, "USB feedback accumulator", 200, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centredRight
        };
        inline static const ColumnHeader AuthorityTableSamplingRate{
            4, "USB audio sampling rate (Hz)", 200, 30, -1,
            juce::TableHeaderComponent::visible | juce::TableHeaderComponent::resizable | juce::TableHeaderComponent::appearsOnColumnMenu,
            juce::Justification::centred
        };

        inline static const ColumnHeader SwitchesTableIpAddress{
            1, "IP address", 150, 30, -1,
            juce::TableHeaderComponent::notSortable,
            juce::Justification::centredLeft
        };
        inline static const ColumnHeader SwitchesTableUsername{
            2, "Username", 100, 30, -1,
            juce::TableHeaderComponent::notSortable,
            juce::Justification::centredLeft
        };
        inline static const ColumnHeader SwitchesTablePassword{
            3, "Password", 100, 30, -1,
            juce::TableHeaderComponent::notSortable,
            juce::Justification::centredLeft
        };
        inline static const ColumnHeader SwitchesTableDrift{
            4, "Freq. drift (ppb)", 100, 30, -1,
            juce::TableHeaderComponent::notSortable,
            juce::Justification::centredRight
        };
        inline static const ColumnHeader SwitchesTableOffset{
            5, "Offset (ns)", 100, 30, -1,
            juce::TableHeaderComponent::notSortable,
            juce::Justification::centredRight
        };
        inline static const ColumnHeader SwitchesTableResetPTP{
            6, "Reset PTP", 75, 30, -1,
            juce::TableHeaderComponent::notSortable,
            juce::Justification::centred
        };
        inline static const ColumnHeader SwitchesTableRemoveSwitch{
            7, "Remove", 75, 30, -1,
            juce::TableHeaderComponent::notSortable,
            juce::Justification::centred
        };
    };

    class Strings
    {
    public:
        inline static const juce::StringRef InputLabel{"Input #"};
        inline static const juce::StringRef OutputLabel{"Output #"};

        inline static const juce::StringRef NetworkTabName{"Network"};
        inline static const juce::StringRef WfsTabName{"WFS"};

        inline static const juce::StringRef SwitchesSectionTitle{"SWITCHES"};
        inline static const juce::StringRef AddSwitchButtonName{"Add switch button"};
        inline static const juce::StringRef AddSwitchButtonText{"+"};

        inline static const juce::StringRef TimeAuthoritySectionTitle{"TIME AUTHORITY"};

        inline static const juce::StringRef ClientsSectionTitle{"CLIENTS"};
        inline static const juce::StringRef RebootAllClientsButtonText{"Reboot all"};
        inline static const juce::StringRef TotalClientsLabel{"Total clients: "};
        inline static const juce::StringRef PresentationTimeIntervalLabel{"Approx. group asynchronicity: "};
    };

    class Identifiers
    {
    public:
        inline static const juce::Identifier StaticTreeType{"WfsParameters"};
        inline static const juce::Identifier DynamicTreeType{"EphemeralData"};
        inline static const juce::Identifier PersistentTreeType{"PersistentData"};
    };

    class Utils
    {
    public:
        static juce::String formatWithThousandsSeparator(const int value)
        {
            std::stringstream ss;
            ss.imbue(std::locale("en_GB.UTF-8"));
            ss << value;
            return ss.str();
        }

        static juce::String formatWithThousandsSeparator(const float value, const int decimals = 3)
        {
            std::stringstream ss;
            ss.imbue(std::locale("en_GB.UTF-8"));
            ss << std::fixed << std::setprecision(decimals) << value;
            return ss.str();
        }
    };
}


#endif //WFSUTILS_H

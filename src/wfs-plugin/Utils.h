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
    enum CommandIDs
    {
        SwitchToWfsTab = 0x1001,
        SwitchToNetworkTab = 0x1002,
        ToggleModuleSelectorDisplay = 0x1003,
    };

    class Constants
    {
    public:
        // For the following, see ananas-client wfsParams.lib
        constexpr static uint MaxChannelsToSend{MAX_CHANNELS_TO_SEND};
        constexpr static uint NumModules{NUM_MODULES};
        constexpr static int MaxYMetres{10};
        constexpr static int MinYMetres{-3};

        inline const static juce::StringRef LocalInterfaceIP{"192.168.10.10"};
        constexpr static int WFSMessengerSocketLocalPort{49160};
        constexpr static int WFSMessengerSocketRemotePort{49160};
        constexpr static int WFSMessengerThreadTimeout{1000};
        inline const static juce::StringRef WFSControlMulticastIP{"224.4.224.10"};

        class UI
        {
        public:
            constexpr static int UiWidth{1200};
            constexpr static int UiHeight{900};

            constexpr static int SwitchesSectionHeight{175};
            constexpr static int TimeAuthoritySectionHeight{112};
            constexpr static int NetworkSectionTitleHeight{40};

            inline static const juce::Colour XYControllerGridlineColour{juce::Colours::lightgrey};

            constexpr static int SpeakerSpacingSectionHeight{50};
            constexpr static float NodeDiameter{40.f};
            constexpr static float NodeHalfDiameter{NodeDiameter / 2.f};

            constexpr static int ModuleSelectorHeight{25};
            constexpr static int ModuleSpeakerHeight{25};
            constexpr static int ModuleHeight{ModuleSelectorHeight + ModuleSpeakerHeight};

            constexpr static float SpeakerIconDimension{100.f};
            constexpr static float SpeakerIconCoilStartX{26.f};
            constexpr static float SpeakerIconCoilStartY{0.f};
            constexpr static float SpeakerIconCoilWidth{SpeakerIconDimension - 2.f * SpeakerIconCoilStartX};
            constexpr static float SpeakerIconCoilHeight{25.f};
            constexpr static float SpeakerIconConeRightX{SpeakerIconDimension};
            constexpr static float SpeakerIconConeEndY{SpeakerIconDimension};
            constexpr static float SpeakerIconConeLeftX{0.f};
            constexpr static float SpeakerIconOutlineThickness{0.f};
            inline static const juce::Colour SpeakerIconFillColour{juce::Colours::ghostwhite};
            inline static const juce::Colour SpeakerIconOutlineColour{juce::Colours::darkgrey};

            constexpr static float OverlayBgAlpha{.25f};
            constexpr static int OverlayBoxWidth{400};
            constexpr static int OverlayBoxHeight{200};
            constexpr static float OverlayBoxBorderRadius{10.f};
            constexpr static float OverlayBoxShadowAlpha{.5f};
            constexpr static int OverlayBoxShadowOffset{4};
            constexpr static float OverlayBoxTextSize{20.f};
            constexpr static float OverlayBoxBorderThickness{5.f};

            constexpr static int TooltipDelayTimeMs{500};

            constexpr static int SwitchToWfsTabKeycode{'1'};
            constexpr static int SwitchToNetworkTabKeycode{'2'};
            constexpr static int ToggleModuleSelectorsKeycode{'m'};
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
        };

        struct RangedParam : Param
        {
            juce::NormalisableRange<float> range;
            juce::NormalisableRange<double> rangeDouble;
            float defaultValue{0.f};
        };

        struct BoolParam : Param
        {
            bool defaultValue{true};
        };

        inline static const RangedParam SpeakerSpacing{
            "/spacing",
            "Speaker Spacing (m)",
            {.05f, .3f, .001f},
            {.05, .3, .001},
            .2
        };

        inline static const BoolParam ShowModuleSelectors{
            "showModuleSelectors",
            "Show module selectors",
            true
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

        inline static const juce::StringRef WfsTabName{"WFS Control"};
        inline static const juce::StringRef NetworkTabName{"Network Overview"};

        inline static const juce::StringRef SwitchesSectionTitle{"SWITCHES"};
        inline static const juce::StringRef AddSwitchButtonName{"Add switch button"};
        inline static const juce::StringRef AddSwitchButtonText{"+"};
        inline static const juce::StringRef AddSwitchButtonTooltip{"Click to add a new switch."};
        inline static const juce::StringRef ResetSwitchButtonText{"Reset"};
        inline static const juce::StringRef RemoveSwitchButtonText{"Remove"};

        inline static const juce::StringRef TimeAuthoritySectionTitle{"TIME AUTHORITY"};

        inline static const juce::StringRef ClientsSectionTitle{"CLIENTS"};
        inline static const juce::StringRef RebootAllClientsButtonText{"Reboot all"};
        inline static const juce::StringRef RebootAllClientsButtonTooltip{"Click to send a reboot instruction to all clients."};
        inline static const juce::StringRef TotalClientsLabel{"Total clients: "};
        inline static const juce::StringRef PresentationTimeIntervalLabel{"Approx. group asynchronicity: "};

        inline static const juce::StringRef WFSMessengerThreadName;

        inline static const juce::String OverlayInitialText{"Looking for network..."};
        inline static const juce::String OverlayNoNetworkText{"No network connection."};

        inline static const juce::StringRef TabsCommandCategoryName{"Tabs"};
        inline static const juce::StringRef SwitchToWfsTabShortName{"WFS Control tab"};
        inline static const juce::StringRef SwitchToWfsTabDescription{"Switches to the WFS Control tab"};
        inline static const juce::StringRef SwitchToNetworkTabShortName{"Network overview tab"};
        inline static const juce::StringRef SwitchToNetworkTabDescription{"Switches to the Network Overview tab"};

        inline static const juce::StringRef OptionsCommandCategoryName{"Options"};
        inline static const juce::StringRef ToggleModuleSelectorsShortName{"Toggle module selectors"};
        inline static const juce::StringRef ToggleModuleSelectorsDescription{"Toggles display of module selector lists"};
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

        static juce::String normalisedPointToCoordinateMetres(const juce::Point<float> p, const juce::Point<float> min, const juce::Point<float> max)
        {
            return "(" +
                   juce::String{p.x * max.x, 3} +
                   " m, " +
                   juce::String{p.y < 0 ? -p.y * min.y : p.y * max.y, 3} +
                   " m)";
        }
    };
}


#endif //WFSUTILS_H

#ifndef WFSUTILS_H
#define WFSUTILS_H

#include <limits.h>
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <AnanasUtils.h>

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
        constexpr static uint NumSources{NUM_SOURCES};
        constexpr static uint NumModules{NUM_MODULES};
        // TODO: receive min/max y-coordinates from clients?
        constexpr static int MaxYMetres{10};
        constexpr static int MinYMetres{-3};

        constexpr static int WFSMessengerThreadTimeout{1000};
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
            return -1.f + (2.f * (static_cast<float>(sourceIndex) + SourcePositionDefaultX) / Constants::NumSources);
        }

        static juce::String getModuleIndexParamID(const int index)
        {
            return "/module/" + juce::String{index};
        }
    };

    class Strings
    {
    public:
        inline static const juce::StringRef WfsTabName{"WFS Control"};
        inline static const juce::StringRef NetworkTabName{"Network Overview"};

        inline static const juce::StringRef TabsCommandCategoryName{"Tabs"};
        inline static const juce::StringRef SwitchToWfsTabShortName{"WFS Control tab"};
        inline static const juce::StringRef SwitchToWfsTabDescription{"Switches to the WFS Control tab"};
        inline static const juce::StringRef SwitchToNetworkTabShortName{"Network overview tab"};
        inline static const juce::StringRef SwitchToNetworkTabDescription{"Switches to the Network Overview tab"};

        inline static const juce::StringRef OptionsCommandCategoryName{"Options"};
        inline static const juce::StringRef ToggleModuleSelectorsShortName{"Toggle module selectors"};
        inline static const juce::StringRef ToggleModuleSelectorsDescription{"Toggles display of module selector lists"};

        inline static const juce::StringRef HideSourceText{"Hide this source"};
        inline static const juce::StringRef HideAllOtherSourcesText{"Hide all except this source"};
    };

    class Identifiers
    {
    public:
        inline static const juce::Identifier StaticTreeType{"WfsParameters"};
    };

    class MenuItems
    {
    public:
        struct MenuItem
        {
            int id{0};
            juce::StringRef text;
        };

        constexpr static int HideSourceMenuID{1};
        constexpr static int HideOtherSourcesMenuID{2};
        constexpr static int ShowAllSourcesMenuID{INT_MAX};

        inline static const MenuItem HideSource{HideSourceMenuID, "Hide this Source"};
        inline static const MenuItem HideOtherSources{HideOtherSourcesMenuID, "Hide all except this Source"};

        inline static const juce::StringRef ShowSourceHeader{"Show source"};
        inline static const MenuItem ShowAllSources{ShowAllSourcesMenuID, "Show all sources"};
    };

    class Utils
    {
    public:
        static juce::String normalisedPointToCoordinateMetres(const juce::Point<float> p, const juce::Point<float> min, const juce::Point<float> max)
        {
            return "(" +
                   juce::String{p.x * max.x, 3} +
                   " m, " +
                   juce::String{p.y < 0 ? -p.y * min.y : p.y * max.y, 3} +
                   " m)";
        }
    };

    class Sockets
    {
    public:
        inline static const ananas::Utils::SenderThreadSocketParams WfsMessengerSocketParams {
            "WFS Messenger",
            500,
            "224.4.224.10",
            49160,
            49160
        };
    };

    namespace UI
    {
        class Dimensions
        {
        public:
            constexpr static int SpeakerSpacingSectionHeight{50};

            constexpr static float NodeDiameter{40.f};
            constexpr static float NodeHalfDiameter{NodeDiameter / 2.f};
            constexpr static float NodeBorderThickness{3.f};
            constexpr static float NodeIndexFontSize{20.f};

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
        };

        class Colours
        {
        public:
            inline static const juce::Colour TabBackgroundColour{juce::Colours::lightgrey};

            inline static const juce::Colour XYControllerGridlineColour{juce::Colours::lightgrey};

            inline static const juce::Colour NodeBgColour{
                juce::Colours::black
                .withSaturation(.5f)
                .withAlpha(.5f)
            };
            inline static const juce::Colour NodeBorderColour{juce::Colours::lightseagreen};
            constexpr static float NodeBorderSaturationRange{.9f};
            constexpr static float NodeBorderBrightnessMin{.2f};
            constexpr static float NodeBorderBrightnessMax{.9f};
            constexpr static float NodeBorderAlpha{.75f};
            inline static const juce::Colour NodeIndexColour{juce::Colours::white};

            inline static const juce::Colour SpeakerIconFillColour{juce::Colours::ghostwhite};
            inline static const juce::Colour SpeakerIconOutlineColour{juce::Colours::darkgrey};
        };

        class Shortcuts
        {
        public:
            constexpr static int SwitchToWfsTabKeycode{'1'};
            constexpr static int SwitchToNetworkTabKeycode{'2'};
            constexpr static int ToggleModuleSelectorsKeycode{'m'};
        };
    }
}

#endif //WFSUTILS_H

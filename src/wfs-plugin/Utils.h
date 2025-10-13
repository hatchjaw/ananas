#ifndef WFSUTILS_H
#define WFSUTILS_H

#include <juce_core/juce_core.h>

#ifndef MAX_CHANNELS_TO_SEND
#define MAX_CHANNELS_TO_SEND 16
#endif

namespace ananas
{
    namespace WFS
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

        class Strings
        {
        public:
            inline static const juce::StringRef InputLabel{"Input #"};
            inline static const juce::StringRef OutputLabel{"Output #"};
        };

        class Identifiers
        {
        public:
            inline static const juce::Identifier StaticTreeType{"WfsParameters"};
            inline static const juce::Identifier DynamicTreeType{"ModuleParameters"};
        };
    }
}


#endif //WFSUTILS_H

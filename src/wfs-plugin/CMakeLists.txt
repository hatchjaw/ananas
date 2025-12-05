set(companyName "Emeraude")

set(companyCode "EMRD")

set(bundleID "org.emeraude.ananas-wfs")

set(juceFormats AU VST3 Standalone)

# Up to 22 channels @ 32 frames
# Up to 44 channels @ 16 frames
set(maxChannels 16)

juce_add_plugin(ananasWFS
        COMPANY_NAME "${companyName}"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE "${companyCode}"
        PLUGIN_CODE Awfs
        FORMATS ${juceFormats}
        VST3_CATEGORIES "Network"
        PRODUCT_NAME "AnanasWFS"
)

clap_juce_extensions_plugin(
        TARGET ananasWFS
        CLAP_ID "${bundleID}"
        CLAP_FEATURES utility
)

target_compile_definitions(ananasWFS
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        MAX_CHANNELS_TO_SEND=${maxChannels}
)

#file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
#target_sources(ananasWFS INTERFACE ${SourceFiles})

target_sources(ananasWFS
        PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp
        UI/LookAndFeel.cpp
        UI/NetworkOverviewComponent.cpp
        UI/SwitchesComponent.cpp
        UI/TimeAuthorityComponent.cpp
        UI/ClientsOverviewComponent.cpp
)

target_link_libraries(ananasWFS
        PRIVATE
        juce::juce_core
        clap_juce_extensions
        ananas_server
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

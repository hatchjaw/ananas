set(COMPANY_NAME "Emeraude")

set(COMPANY_CODE "EMRD")

set(BUNDLE_ID "org.emeraude.ananas-wfs")

set(JUCE_FORMATS AU VST3 Standalone)

juce_add_plugin(ananasWFS
        COMPANY_NAME "${COMPANY_NAME}"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE "${COMPANY_CODE}"
        PLUGIN_CODE Awfs
        FORMATS ${JUCE_FORMATS}
        VST3_CATEGORIES "Network"
        PRODUCT_NAME "AnanasWFS"
)

clap_juce_extensions_plugin(
        TARGET ananasWFS
        CLAP_ID "${BUNDLE_ID}"
        CLAP_FEATURES utility
)

target_compile_definitions(ananasWFS
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        AUDIO_SAMPLE_RATE=${audioSampleRate}
)

#file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
#target_sources(ananasWFS INTERFACE ${SourceFiles})

target_sources(ananasWFS
        PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp
        Utils.cpp
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

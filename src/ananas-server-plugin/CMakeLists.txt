set(companyName "Emeraude")

set(companyCode "EMRD")

set(pluginCode Asrv)

set(bundleID "org.emeraude.ananas-server")

set(juceFormats Standalone)

set(maxChannels 2)

juce_add_plugin(ananasServer
        COMPANY_NAME "${companyName}"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE "${companyCode}"
        PLUGIN_CODE ${PluginCode}
        FORMATS ${juceFormats}
        PRODUCT_NAME "AnanasServer"
)

clap_juce_extensions_plugin(
        TARGET ananasServer
        CLAP_ID "${bundleID}"
        CLAP_FEATURES utility
)

target_compile_definitions(ananasServer
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        MAX_CHANNELS_TO_SEND=${maxChannels}
        SHOW_NO_NETWORK_OVERLAY=$<BOOL:${SHOW_NO_NETWORK_OVERLAY}>
)

target_sources(ananasServer
        PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp
)

target_link_libraries(ananasServer
        PRIVATE
        juce::juce_core
        juce::juce_audio_devices
        juce::juce_audio_utils
        clap_juce_extensions
        ananas_server
        ananas_network_overview
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

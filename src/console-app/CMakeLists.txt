juce_add_console_app(ananas_console
        PRODUCT_NAME "Ananas Console")

target_compile_definitions(ananas_console
        PRIVATE
        JUCE_JACK=1
        JUCE_JACK_CLIENT_NAME="${PROJECT_NAME}"
        DEFAULT_LOCAL_ADDRESS="192.168.10.10"
        DEFAULT_LOCAL_PORT=14841
        DEFAULT_MULTICAST_IP="224.4.224.4"
        DEFAULT_AUDIO_SEND_PORT=6664)

target_sources(ananas_console
        PRIVATE
        Main.cpp
        MainComponent.cpp)

target_compile_definitions(ananas_console
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:ananas_console,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:ananas_console,JUCE_VERSION>")

target_link_libraries(ananas_console
        PRIVATE
        juce::juce_core
        juce::juce_audio_utils
        ananas_server
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

set(audioSampleRate 48000)

juce_add_console_app(ananas_console
        PRODUCT_NAME "Ananas Console")

target_compile_definitions(ananas_console
        PRIVATE
        JUCE_JACK=1
        JUCE_JACK_CLIENT_NAME="${PROJECT_NAME}")

target_sources(ananas_console
        PRIVATE
        Main.cpp
        MainComponent.cpp)

target_compile_definitions(ananas_console
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:ananas_console,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:ananas_console,JUCE_VERSION>"
        AUDIO_SAMPLE_RATE=${audioSampleRate})

target_link_libraries(ananas_console
        PRIVATE
        juce::juce_core
        juce::juce_audio_utils
        ananas_server
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

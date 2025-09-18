add_library(ananas_server
        Server.cpp
        Fifo.cpp
        Packet.cpp
)

target_include_directories(ananas_server
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ananas_server
        PRIVATE
        juce::juce_audio_basics
)

target_compile_definitions(ananas_server
        PRIVATE
        AUDIO_SAMPLE_RATE=${audioSampleRate}
)

set_target_properties(ananas_server PROPERTIES POSITION_INDEPENDENT_CODE ON)

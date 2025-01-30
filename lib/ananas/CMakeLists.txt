add_library(ananas_server
        AnanasServer.cpp
        AnanasFifo.cpp
        AnanasPacket.cpp
        AnanasPacket.h
)

target_include_directories(ananas_server
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ananas_server
        PRIVATE
        juce::juce_audio_basics
)

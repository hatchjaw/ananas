add_library(ananas_ui STATIC
        LookAndFeel.cpp
)

target_include_directories(ananas_ui
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ananas_ui
        PUBLIC
        juce::juce_gui_basics
)

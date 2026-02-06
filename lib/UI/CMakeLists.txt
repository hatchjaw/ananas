add_library(ananas_network_overview
        OverlayableComponent.cpp
        network/AnanasNetworkTable.cpp
        network/NetworkOverviewComponent.cpp
        network/SwitchesComponent.cpp
        network/TimeAuthorityComponent.cpp
        network/ClientsOverviewComponent.cpp
)

set_target_properties(ananas_network_overview PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        VERSION 0.1.0
        SOVERSION 1
)

target_include_directories(ananas_network_overview
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(ananas_network_overview
        PRIVATE
        SHOW_NO_NETWORK_OVERLAY=$<BOOL:${SHOW_NO_NETWORK_OVERLAY}>
)

target_link_libraries(ananas_network_overview
        PRIVATE
        juce::juce_core
        ananas_server
)

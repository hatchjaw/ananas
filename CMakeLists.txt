cmake_minimum_required(VERSION 3.30)
project(ananas VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(JUCE)

add_subdirectory(clap-juce-extensions EXCLUDE_FROM_ALL)

add_subdirectory(lib/ananas-utilities)

add_subdirectory(lib/ananas-server)

add_subdirectory(lib/network-ui)

add_subdirectory(src/console-app)

option(SHOW_NO_NETWORK_OVERLAY "Show UI overlay if a network connection cannot be established" ON)

add_subdirectory(src/ananas-server-plugin)

add_subdirectory(src/wfs-plugin)

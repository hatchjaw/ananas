cmake_minimum_required(VERSION 3.30)
project(ananas VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)

set(audioSampleRate 48000)

add_subdirectory(JUCE)

add_subdirectory(clap-juce-extensions EXCLUDE_FROM_ALL)

add_subdirectory(lib/ananas)

add_subdirectory(src/console-app)

add_subdirectory(src/wfs-plugin)

add_subdirectory(src/clockcheck)

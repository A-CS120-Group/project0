cmake_minimum_required(VERSION 3.23)

set(CURRENT_TARGET "Project0_Part1")

project(${CURRENT_TARGET} VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(JUCE)

juce_add_gui_app(${CURRENT_TARGET} PRODUCT_NAME ${CURRENT_TARGET})

juce_generate_juce_header(${CURRENT_TARGET})

target_sources(${CURRENT_TARGET}
        PRIVATE
        part1/main.cpp
        part1/Part1.h
        part1/Part1.cpp
        )


target_compile_definitions(${CURRENT_TARGET}
        PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        )

target_link_libraries(${CURRENT_TARGET}
        PRIVATE
        juce::juce_analytics
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_box2d
        juce::juce_core
        juce::juce_cryptography
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_opengl
        juce::juce_osc
        juce::juce_product_unlocking
        juce::juce_video
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)

find_package(Boost COMPONENTS system filesystem REQUIRED)
target_link_libraries(${CURRENT_TARGET} PRIVATE Boost::filesystem)

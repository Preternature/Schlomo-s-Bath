cmake_minimum_required(VERSION 3.15)

project(SchlomosBath VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add JUCE
if(NOT DEFINED JUCE_DIR)
    if(EXISTS "C:/JUCE")
        set(JUCE_DIR "C:/JUCE")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/JUCE")
        set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/JUCE")
    else()
        message(FATAL_ERROR "JUCE not found. Please set JUCE_DIR or clone JUCE into this directory")
    endif()
endif()

message(STATUS "Using JUCE from: ${JUCE_DIR}")
add_subdirectory(${JUCE_DIR} ${CMAKE_BINARY_DIR}/JUCE)

# Create the plugin target
juce_add_plugin(SchlomosBath
    COMPANY_NAME "Schlomo"
    PLUGIN_MANUFACTURER_CODE Schl
    PLUGIN_CODE Bath
    FORMATS VST3
    PRODUCT_NAME "Schlomos Bath"
    VST3_CATEGORIES Fx Modulation
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_SYNTH FALSE
    IS_MIDI_EFFECT FALSE
    PLUGIN_MANUFACTURER_EMAIL "info@schlomo.audio"
    PLUGIN_DESCRIPTION "Vocal Life Engine - Bathtub personality simulator for vocals"
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
)

# Add source files
target_sources(SchlomosBath
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/VocalProcessor.cpp
        libs/rubberband/single/RubberBandSingle.cpp
)

# RubberBand configuration
target_compile_definitions(SchlomosBath
    PRIVATE
        USE_KISSFFT=1
        USE_BUILTIN_FFT=1
        NOMINMAX
)

# Link JUCE modules
target_link_libraries(SchlomosBath
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compiler definitions
target_compile_definitions(SchlomosBath
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DSP_USE_INTEL_MKL=0
)

# Include directories
target_include_directories(SchlomosBath
    PRIVATE
        Source
        libs/rubberband
)

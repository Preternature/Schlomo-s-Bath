#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
class SchlomosBathAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SchlomosBathAudioProcessorEditor (SchlomosBathAudioProcessor&);
    ~SchlomosBathAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SchlomosBathAudioProcessor& audioProcessor;

    // UI Components
    juce::Label titleLabel;
    juce::Label subtitleLabel;

    // Master controls
    juce::Label masterMixLabel;
    juce::Slider masterMixSlider;

    // Parameter sliders with labels
    // Category 1: Human Vocal Randomizers
    juce::Label pitchDriftLabel{"", "U-Bend Shift"};
    juce::Slider pitchDriftSlider;
    juce::Label formantLabel{"", "Formant Wobble"};
    juce::Slider formantSlider;
    juce::Label breathLabel{"", "Breath Noise"};
    juce::Slider breathSlider;
    juce::Label timingLabel{"", "Timing Wobble"};
    juce::Slider timingSlider;
    juce::Label volumeLabel{"", "Volume Wobble"};
    juce::Slider volumeSlider;

    // Category 2: Environmental
    juce::Label porcelainLabel{"", "Porcelain Reflect"};
    juce::Slider porcelainSlider;
    juce::Label steamLabel{"", "Steam/Humidity"};
    juce::Slider steamSlider;

    // Category 3: Character Modes
    juce::Label quackLabel{"", "Rubber Duck"};
    juce::Slider quackSlider;
    juce::Label soapLabel{"", "Soap Glitch"};
    juce::Slider soapSlider;

    // Scrollable viewport for controls
    juce::Viewport controlsViewport;
    std::unique_ptr<juce::Component> controlsContainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SchlomosBathAudioProcessorEditor)
};

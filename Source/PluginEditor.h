#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
// LFO Visualizer Component - shows sine wave with current phase indicator
class LFOVisualizer : public juce::Component
{
public:
    LFOVisualizer() = default;

    void setPhase(float newPhase) { phase = newPhase; repaint(); }
    void setCurrentValue(float value) { currentValue = value; repaint(); }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2);

        // Background
        g.setColour(juce::Colour(0xff0a0a15));
        g.fillRoundedRectangle(bounds, 4.0f);

        // Draw sine wave
        g.setColour(juce::Colour(0xff00ffff).withAlpha(0.5f));
        juce::Path wavePath;

        float width = bounds.getWidth();
        float height = bounds.getHeight();
        float centerY = bounds.getY() + height / 2;
        float amplitude = height / 2.5f;

        wavePath.startNewSubPath(bounds.getX(), centerY);
        for (float x = 0; x < width; x += 2)
        {
            float normalizedX = x / width;
            float y = centerY - std::sin(normalizedX * juce::MathConstants<float>::twoPi) * amplitude;
            wavePath.lineTo(bounds.getX() + x, y);
        }
        g.strokePath(wavePath, juce::PathStrokeType(1.5f));

        // Draw current phase position (vertical line)
        float phaseX = bounds.getX() + phase * width;
        g.setColour(juce::Colour(0xffff6600));
        g.drawVerticalLine((int)phaseX, bounds.getY(), bounds.getBottom());

        // Draw current value indicator (dot on the wave)
        float dotY = centerY - std::sin(phase * juce::MathConstants<float>::twoPi) * amplitude;
        g.setColour(juce::Colour(0xffff6600));
        g.fillEllipse(phaseX - 4, dotY - 4, 8, 8);

        // Draw cents indicator text
        g.setColour(juce::Colours::white);
        g.setFont(10.0f);
        juce::String centsText = juce::String(currentValue, 1) + " ct";
        g.drawText(centsText, bounds.toNearestInt(), juce::Justification::bottomRight);
    }

private:
    float phase = 0.0f;
    float currentValue = 0.0f;
};

//==============================================================================
class SchlomosBathAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                          private juce::Timer
{
public:
    SchlomosBathAudioProcessorEditor (SchlomosBathAudioProcessor&);
    ~SchlomosBathAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDoubleClick (const juce::MouseEvent& event) override;
    void timerCallback() override;

private:
    void toggleFullscreen();

    bool isFullscreen = false;
    int previousWidth = 800;
    int previousHeight = 600;

    SchlomosBathAudioProcessor& audioProcessor;

    // UI Components
    juce::Label titleLabel;
    juce::Label subtitleLabel;

    // Master controls
    juce::Label masterMixLabel;
    juce::Slider masterMixSlider;

    // Parameter sliders with labels
    // Category 1: Human Vocal Randomizers
    // U-Bend section
    juce::Label uBendSectionLabel{"", "U-BEND"};
    juce::Label pitchDriftLabel{"", "Amount (0-100 cents)"};
    juce::Slider pitchDriftSlider;
    juce::Label lfoSpeedLabel{"", "LFO Speed"};
    juce::Slider lfoSpeedSlider;
    LFOVisualizer uBendVisualizer;
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

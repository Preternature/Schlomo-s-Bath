#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SchlomosBathAudioProcessorEditor::SchlomosBathAudioProcessorEditor (SchlomosBathAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Title
    titleLabel.setText("SCHLOMO'S BATH", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(32.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xff00ffff));
    addAndMakeVisible(titleLabel);

    // Subtitle
    subtitleLabel.setText("Vocal Life Engine", juce::dontSendNotification);
    subtitleLabel.setFont(juce::Font(14.0f, juce::Font::italic));
    subtitleLabel.setJustificationType(juce::Justification::centred);
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(subtitleLabel);

    // Master Mix
    masterMixLabel.setText("Master Mix", juce::dontSendNotification);
    masterMixLabel.setFont(juce::Font(12.0f));
    masterMixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(masterMixLabel);

    masterMixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    masterMixSlider.setRange(0.0, 1.0, 0.01);
    masterMixSlider.setValue(1.0);
    masterMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    masterMixSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().setMasterMix((float)masterMixSlider.getValue());
    };
    addAndMakeVisible(masterMixSlider);
    audioProcessor.getVocalProcessor().setMasterMix(1.0f);

    // Enable all modules by default (slider controls amount)
    audioProcessor.getVocalProcessor().getPitchDriftBrain().setEnabled(true);
    audioProcessor.getVocalProcessor().getFormantWhispers().setEnabled(true);
    audioProcessor.getVocalProcessor().getBreathNoiseEngine().setEnabled(true);
    audioProcessor.getVocalProcessor().getTimingWobble().setEnabled(true);
    audioProcessor.getVocalProcessor().getVolumePersonality().setEnabled(true);
    audioProcessor.getVocalProcessor().getPorcelainReflections().setEnabled(true);
    audioProcessor.getVocalProcessor().getSteamModulator().setEnabled(true);
    audioProcessor.getVocalProcessor().getRubberDuckFM().setEnabled(true);
    audioProcessor.getVocalProcessor().getSoapBarGlitch().setEnabled(true);

    // Helper to setup sliders
    auto setupSlider = [this](juce::Slider& slider, juce::Label& label) {
        label.setJustificationType(juce::Justification::centredLeft);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label);

        slider.setSliderStyle(juce::Slider::LinearHorizontal);
        slider.setRange(0.0, 1.0, 0.01);
        slider.setValue(0.0);
        slider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
        addAndMakeVisible(slider);
    };

    // Category 1: Human Vocal Randomizers
    // U-Bend section header
    uBendSectionLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    uBendSectionLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff6600));
    uBendSectionLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(uBendSectionLabel);

    setupSlider(pitchDriftSlider, pitchDriftLabel);
    pitchDriftSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getPitchDriftBrain().setIntensity((float)pitchDriftSlider.getValue());
    };

    setupSlider(lfoSpeedSlider, lfoSpeedLabel);
    lfoSpeedSlider.setValue(0.3);  // Default to moderate speed
    lfoSpeedSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getPitchDriftBrain().setLFOSpeed((float)lfoSpeedSlider.getValue());
    };

    // U-Bend LFO Visualizer
    addAndMakeVisible(uBendVisualizer);

    // Start timer for visualizer updates (30 FPS)
    startTimerHz(30);

    setupSlider(formantSlider, formantLabel);
    formantSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getFormantWhispers().setMouthWobble((float)formantSlider.getValue());
    };

    setupSlider(breathSlider, breathLabel);
    breathSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getBreathNoiseEngine().setBreathIntensity((float)breathSlider.getValue());
    };

    setupSlider(timingSlider, timingLabel);
    timingSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getTimingWobble().setWobbleAmount((float)timingSlider.getValue());
    };

    setupSlider(volumeSlider, volumeLabel);
    volumeSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getVolumePersonality().setIntensity((float)volumeSlider.getValue());
    };

    // Category 2: Environmental
    setupSlider(porcelainSlider, porcelainLabel);
    porcelainSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getPorcelainReflections().setTileScatter((float)porcelainSlider.getValue());
    };

    setupSlider(steamSlider, steamLabel);
    steamSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getSteamModulator().setHumidity((float)steamSlider.getValue());
    };

    // Category 3: Character Modes
    setupSlider(quackSlider, quackLabel);
    quackSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getRubberDuckFM().setQuackIntensity((float)quackSlider.getValue());
    };

    setupSlider(soapSlider, soapLabel);
    soapSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getSoapBarGlitch().setSlipperiness((float)soapSlider.getValue());
    };

    // Make resizable
    setResizable(true, true);
    setResizeLimits(800, 600, 3840, 2160);
    setSize(800, 600);
}

SchlomosBathAudioProcessorEditor::~SchlomosBathAudioProcessorEditor()
{
    stopTimer();
}

void SchlomosBathAudioProcessorEditor::timerCallback()
{
    // Update U-Bend LFO visualizer
    auto& pitchBrain = audioProcessor.getVocalProcessor().getPitchDriftBrain();
    uBendVisualizer.setPhase(pitchBrain.getLFOPhase());
    uBendVisualizer.setCurrentValue(pitchBrain.getCurrentCents());
}

//==============================================================================
void SchlomosBathAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Background gradient
    g.fillAll(juce::Colour(0xff1a1a2e));

    auto bounds = getLocalBounds();
    float width = (float)bounds.getWidth();
    float height = (float)bounds.getHeight();

    // Bathtub graphic at bottom
    g.setColour(juce::Colour(0xff2d2d44));
    float tubMargin = width * 0.0625f;
    float tubY = height * 0.75f;
    float tubHeight = height * 0.2f;
    g.fillRoundedRectangle(tubMargin, tubY, width - 2 * tubMargin, tubHeight, 20.0f);

    // Water in tub
    g.setColour(juce::Colour(0xff4a90a4));
    g.fillRoundedRectangle(tubMargin + 10, tubY + 10, width - 2 * tubMargin - 20, tubHeight - 20, 15.0f);

    // Steam wisps
    g.setColour(juce::Colour(0x30ffffff));
    for (int i = 0; i < 5; ++i)
    {
        float x = tubMargin + 50 + i * (width - 2 * tubMargin - 100) / 4;
        float steamY = tubY - 30 - (i % 2) * 20;
        g.fillEllipse(x, steamY, 30, 15);
    }

    // Category headers
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.setColour(juce::Colour(0xff00ffff));

    auto area = bounds.reduced(20);
    area.removeFromTop(80);

    auto moduleArea = area.removeFromTop(400);
    auto col1 = moduleArea.removeFromLeft(250);
    auto col2 = moduleArea.removeFromLeft(250);
    auto col3 = moduleArea;

    g.drawText("HUMAN VOCAL", col1.removeFromTop(25), juce::Justification::centredLeft);
    g.drawText("ENVIRONMENT", col2.removeFromTop(25), juce::Justification::centredLeft);
    g.drawText("CHARACTER", col3.removeFromTop(25), juce::Justification::centredLeft);
}

void SchlomosBathAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    auto area = bounds.reduced(20);

    // Title area
    titleLabel.setBounds(area.removeFromTop(40));
    subtitleLabel.setBounds(area.removeFromTop(20));
    area.removeFromTop(20);

    // Module sliders - 3 columns
    auto moduleArea = area.removeFromTop(400);

    // Column 1: Human Vocal (left)
    auto col1 = moduleArea.removeFromLeft(250).reduced(5);
    col1.removeFromTop(25); // Header space

    // U-Bend section
    uBendSectionLabel.setBounds(col1.removeFromTop(18));
    pitchDriftLabel.setBounds(col1.removeFromTop(18));
    pitchDriftSlider.setBounds(col1.removeFromTop(22));
    lfoSpeedLabel.setBounds(col1.removeFromTop(18));
    lfoSpeedSlider.setBounds(col1.removeFromTop(22));
    uBendVisualizer.setBounds(col1.removeFromTop(50));
    col1.removeFromTop(8);

    formantLabel.setBounds(col1.removeFromTop(20));
    formantSlider.setBounds(col1.removeFromTop(25));
    col1.removeFromTop(5);

    breathLabel.setBounds(col1.removeFromTop(20));
    breathSlider.setBounds(col1.removeFromTop(25));
    col1.removeFromTop(5);

    timingLabel.setBounds(col1.removeFromTop(20));
    timingSlider.setBounds(col1.removeFromTop(25));
    col1.removeFromTop(5);

    volumeLabel.setBounds(col1.removeFromTop(20));
    volumeSlider.setBounds(col1.removeFromTop(25));

    // Column 2: Environment (middle)
    auto col2 = moduleArea.removeFromLeft(250).reduced(5);
    col2.removeFromTop(25); // Header space

    porcelainLabel.setBounds(col2.removeFromTop(20));
    porcelainSlider.setBounds(col2.removeFromTop(25));
    col2.removeFromTop(5);

    steamLabel.setBounds(col2.removeFromTop(20));
    steamSlider.setBounds(col2.removeFromTop(25));

    // Column 3: Character (right)
    auto col3 = moduleArea.reduced(5);
    col3.removeFromTop(25); // Header space

    quackLabel.setBounds(col3.removeFromTop(20));
    quackSlider.setBounds(col3.removeFromTop(25));
    col3.removeFromTop(5);

    soapLabel.setBounds(col3.removeFromTop(20));
    soapSlider.setBounds(col3.removeFromTop(25));

    // Master mix at bottom center
    auto mixArea = area.removeFromTop(100).withSizeKeepingCentre(150, 100);
    masterMixLabel.setBounds(mixArea.removeFromTop(20));
    masterMixSlider.setBounds(mixArea);
}

void SchlomosBathAudioProcessorEditor::mouseDoubleClick(const juce::MouseEvent& event)
{
    // Check if double-click was in the title banner area (top 80 pixels)
    if (event.y < 80)
    {
        toggleFullscreen();
    }
}

void SchlomosBathAudioProcessorEditor::toggleFullscreen()
{
    if (isFullscreen)
    {
        // Restore previous size
        setSize(previousWidth, previousHeight);
        isFullscreen = false;
    }
    else
    {
        // Save current size
        previousWidth = getWidth();
        previousHeight = getHeight();

        // Get screen bounds and set to max allowed size
        auto display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay();
        if (display != nullptr)
        {
            auto screenBounds = display->userArea;
            // Use the resize limits we set (max 3840x2160)
            int maxWidth = juce::jmin(screenBounds.getWidth(), 3840);
            int maxHeight = juce::jmin(screenBounds.getHeight(), 2160);
            setSize(maxWidth, maxHeight);
        }
        else
        {
            // Fallback to our max limits
            setSize(3840, 2160);
        }
        isFullscreen = true;
    }
}

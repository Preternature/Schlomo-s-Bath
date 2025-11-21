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

    // Low cents slider (-50 to 0)
    centsLowLabel.setJustificationType(juce::Justification::centredLeft);
    centsLowLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(centsLowLabel);
    centsLowSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    centsLowSlider.setRange(-150.0, 0.0, 1.0);
    centsLowSlider.setValue(0.0);
    centsLowSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    centsLowSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getPitchDriftBrain().setCentsLow((float)centsLowSlider.getValue());
    };
    addAndMakeVisible(centsLowSlider);

    // High cents slider (0 to +50)
    centsHighLabel.setJustificationType(juce::Justification::centredLeft);
    centsHighLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(centsHighLabel);
    centsHighSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    centsHighSlider.setRange(0.0, 150.0, 1.0);
    centsHighSlider.setValue(0.0);
    centsHighSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    centsHighSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getPitchDriftBrain().setCentsHigh((float)centsHighSlider.getValue());
    };
    addAndMakeVisible(centsHighSlider);

    setupSlider(lfoSpeedSlider, lfoSpeedLabel);
    lfoSpeedSlider.setValue(0.3);  // Default to moderate speed
    lfoSpeedSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getPitchDriftBrain().setLFOSpeed((float)lfoSpeedSlider.getValue());
    };

    // Randomize mode toggle (randomize vs high/low)
    randomizeModeToggle.setToggleState(true, juce::dontSendNotification);  // Default to randomize
    randomizeModeToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    randomizeModeToggle.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xffff6600));
    randomizeModeToggle.onClick = [this] {
        audioProcessor.getVocalProcessor().getPitchDriftBrain().setRandomizeMode(randomizeModeToggle.getToggleState());
    };
    addAndMakeVisible(randomizeModeToggle);

    // U-Bend LFO Visualizer
    addAndMakeVisible(uBendVisualizer);

    // Start timer for visualizer updates (30 FPS)
    startTimerHz(30);

    // Formant section
    formantSectionLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    formantSectionLabel.setColour(juce::Label::textColourId, juce::Colour(0xffff6600));
    formantSectionLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(formantSectionLabel);

    // Formant low slider (-1 to 0)
    formantLowLabel.setJustificationType(juce::Justification::centredLeft);
    formantLowLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(formantLowLabel);
    formantLowSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    formantLowSlider.setRange(-5.0, 0.0, 0.01);
    formantLowSlider.setValue(0.0);
    formantLowSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    formantLowSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getFormantWhispers().setFormantShiftLow((float)formantLowSlider.getValue());
    };
    addAndMakeVisible(formantLowSlider);

    // Formant high slider (0 to +1)
    formantHighLabel.setJustificationType(juce::Justification::centredLeft);
    formantHighLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(formantHighLabel);
    formantHighSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    formantHighSlider.setRange(0.0, 5.0, 0.01);
    formantHighSlider.setValue(0.0);
    formantHighSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 40, 20);
    formantHighSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getFormantWhispers().setFormantShiftHigh((float)formantHighSlider.getValue());
    };
    addAndMakeVisible(formantHighSlider);

    // Formant LFO speed
    setupSlider(formantLFOSpeedSlider, formantLFOSpeedLabel);
    formantLFOSpeedSlider.setValue(0.3);
    formantLFOSpeedSlider.onValueChange = [this] {
        audioProcessor.getVocalProcessor().getFormantWhispers().setFormantLFOSpeed((float)formantLFOSpeedSlider.getValue());
    };

    // Formant randomize toggle
    formantRandomizeModeToggle.setToggleState(true, juce::dontSendNotification);
    formantRandomizeModeToggle.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    formantRandomizeModeToggle.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xffff6600));
    formantRandomizeModeToggle.onClick = [this] {
        audioProcessor.getVocalProcessor().getFormantWhispers().setFormantRandomizeMode(formantRandomizeModeToggle.getToggleState());
    };
    addAndMakeVisible(formantRandomizeModeToggle);

    // Formant LFO visualizer
    addAndMakeVisible(formantVisualizer);

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

    // Update Formant LFO visualizer
    auto& formantWhispers = audioProcessor.getVocalProcessor().getFormantWhispers();
    formantVisualizer.setPhase(formantWhispers.getFormantLFOPhase());
    formantVisualizer.setCurrentValue(formantWhispers.getCurrentFormantShift() * 100.0f);  // Scale for display
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
    auto moduleArea = area.removeFromTop(500);

    // Column 1: Human Vocal (left)
    auto col1 = moduleArea.removeFromLeft(250).reduced(5);
    col1.removeFromTop(25); // Header space

    // U-Bend section
    uBendSectionLabel.setBounds(col1.removeFromTop(18));
    centsLowLabel.setBounds(col1.removeFromTop(16));
    centsLowSlider.setBounds(col1.removeFromTop(20));
    centsHighLabel.setBounds(col1.removeFromTop(16));
    centsHighSlider.setBounds(col1.removeFromTop(20));
    lfoSpeedLabel.setBounds(col1.removeFromTop(16));
    lfoSpeedSlider.setBounds(col1.removeFromTop(20));
    randomizeModeToggle.setBounds(col1.removeFromTop(20));
    uBendVisualizer.setBounds(col1.removeFromTop(40));
    col1.removeFromTop(5);

    // Formant section
    formantSectionLabel.setBounds(col1.removeFromTop(18));
    formantLowLabel.setBounds(col1.removeFromTop(16));
    formantLowSlider.setBounds(col1.removeFromTop(20));
    formantHighLabel.setBounds(col1.removeFromTop(16));
    formantHighSlider.setBounds(col1.removeFromTop(20));
    formantLFOSpeedLabel.setBounds(col1.removeFromTop(16));
    formantLFOSpeedSlider.setBounds(col1.removeFromTop(20));
    formantRandomizeModeToggle.setBounds(col1.removeFromTop(20));
    formantVisualizer.setBounds(col1.removeFromTop(40));

    // Column 2: Environment (middle)
    auto col2 = moduleArea.removeFromLeft(250).reduced(5);
    col2.removeFromTop(25); // Header space

    breathLabel.setBounds(col2.removeFromTop(20));
    breathSlider.setBounds(col2.removeFromTop(25));
    col2.removeFromTop(5);

    timingLabel.setBounds(col2.removeFromTop(20));
    timingSlider.setBounds(col2.removeFromTop(25));
    col2.removeFromTop(5);

    volumeLabel.setBounds(col2.removeFromTop(20));
    volumeSlider.setBounds(col2.removeFromTop(25));
    col2.removeFromTop(10);

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

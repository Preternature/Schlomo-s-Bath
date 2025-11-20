#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <rubberband/RubberBandLiveShifter.h>
#include <memory>

//==============================================================================
// Base class for all vocal processing modules
class VocalModule
{
public:
    VocalModule() = default;
    virtual ~VocalModule() = default;

    // Prepare the module for processing
    virtual void prepare(double sampleRate, int samplesPerBlock) = 0;

    // Process audio in-place
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;

    // Reset internal state
    virtual void reset() = 0;

    // Module enable/disable
    void setEnabled(bool shouldBeEnabled) { enabled = shouldBeEnabled; }
    bool isEnabled() const { return enabled; }

    // Wet/dry mix (0.0 = dry, 1.0 = wet)
    void setMix(float newMix) { mix = juce::jlimit(0.0f, 1.0f, newMix); }
    float getMix() const { return mix; }

    // Module name for UI
    virtual juce::String getName() const = 0;

protected:
    bool enabled = false;  // Disabled by default
    float mix = 1.0f;
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
};

//==============================================================================
// MODULE CATEGORY 1: Human Vocal Randomizers

//==============================================================================
// 1. Pitch Drift Brain™
// Behavior-driven pitch variation with vowel-based glides and searching
class PitchDriftBrain : public VocalModule
{
public:
    PitchDriftBrain();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    juce::String getName() const override { return "Pitch Drift Brain"; }

    // Behavior modes
    enum BehaviorMode {
        VowelGlides,
        TransientOvershoot,
        PitchSearching,
        DrunkMode,
        AnxietyMode
    };

    void setIntensity(float intensity) { this->intensity = juce::jlimit(0.0f, 1.0f, intensity); }
    void setLFOSpeed(float speed) { lfoSpeed = juce::jlimit(0.0f, 1.0f, speed); }

    // Getters for visualizers
    float getLFOPhase() const { return lfoPhase; }
    float getCurrentCents() const { return currentCents; }
    float getTargetCents() const { return targetCents; }

private:
    float intensity = 0.0f;  // 0-1 maps to 0-100 cents range
    float lfoSpeed = 0.3f;   // LFO speed (0.1 Hz to 5 Hz)

    // LFO state
    float lfoPhase = 0.0f;
    bool wasPositive = true;  // Track LFO zero crossings

    // Pitch state
    float currentCents = 0.0f;   // Current detuning in cents
    float targetCents = 0.0f;    // Target detuning (picked at LFO peaks/valleys)

    // RubberBand pitch shifter (one per channel for stereo)
    std::vector<std::unique_ptr<RubberBand::RubberBandLiveShifter>> shifters;
    std::vector<std::vector<float>> inputBuffers;
    std::vector<std::vector<float>> outputBuffers;
    std::vector<std::vector<float>> outputFIFOs;  // FIFO for output samples
    std::vector<size_t> inputPos;   // Write position in input buffer
    std::vector<size_t> outputPos;  // Read position in output FIFO
    std::vector<size_t> outputAvailable;  // Samples available in output FIFO
    size_t rbBlockSize = 0;
    int numChannels = 0;

    juce::Random random;
};

//==============================================================================
// 2. Formant Whispers
// Random low-level formant shifts (nasalization, throat resonance, etc.)
class FormantWhispers : public VocalModule
{
public:
    FormantWhispers();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    juce::String getName() const override { return "Formant Whispers"; }

    void setNasalization(float amount) { nasalization = juce::jlimit(0.0f, 1.0f, amount); }
    void setThroatShift(float amount) { throatShift = juce::jlimit(-1.0f, 1.0f, amount); }
    void setMouthWobble(float amount) { mouthWobble = juce::jlimit(0.0f, 1.0f, amount); }

private:
    float nasalization = 0.0f;
    float throatShift = 0.0f;
    float mouthWobble = 0.0f;

    // Formant filter banks
    juce::dsp::ProcessorChain<juce::dsp::IIR::Filter<float>,
                              juce::dsp::IIR::Filter<float>,
                              juce::dsp::IIR::Filter<float>> formantFilters;
    juce::Random random;
};

//==============================================================================
// 3. Breath & Noise Engine
// Context-aware breath and noise addition
class BreathNoiseEngine : public VocalModule
{
public:
    BreathNoiseEngine();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    juce::String getName() const override { return "Breath & Noise Engine"; }

    void setBreathIntensity(float intensity) { breathIntensity = juce::jlimit(0.0f, 1.0f, intensity); }
    void setHuffMode(bool enabled) { huffMode = enabled; }

private:
    float breathIntensity = 0.0f;
    bool huffMode = false;

    juce::Random random;
    juce::dsp::IIR::Filter<float> breathFilter;
    float envelopeFollower = 0.0f;
};

//==============================================================================
// 4. Timing Wobble
// Micro-timing deviations for human-like imperfection
class TimingWobble : public VocalModule
{
public:
    TimingWobble();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    juce::String getName() const override { return "Timing Wobble"; }

    void setWobbleAmount(float amount) { wobbleAmount = juce::jlimit(0.0f, 1.0f, amount); }
    void setSwingFeel(float swing) { swingFeel = juce::jlimit(0.0f, 1.0f, swing); }

private:
    float wobbleAmount = 0.0f;
    float swingFeel = 0.0f;

    juce::dsp::DelayLine<float> timingBuffer{44100};
    juce::Random random;
    float currentDelay = 0.0f;
};

//==============================================================================
// 5. Volume Personality
// Human-like fader movement
class VolumePersonality : public VocalModule
{
public:
    VolumePersonality();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    juce::String getName() const override { return "Volume Personality"; }

    enum PersonalityType {
        Nervous,
        Confident,
        Wavering,
        TikTokCompression
    };

    void setPersonality(PersonalityType type) { personalityType = type; }
    void setIntensity(float intensity) { this->intensity = juce::jlimit(0.0f, 1.0f, intensity); }

private:
    PersonalityType personalityType = Wavering;
    float intensity = 0.0f;

    juce::Random random;
    float currentGain = 1.0f;
    float targetGain = 1.0f;
};

//==============================================================================
// MODULE CATEGORY 2: Environmental / Bathtub Randomizers

//==============================================================================
// 6. Porcelain Reflections Engine
// Chaotic early reflections, not static reverb
class PorcelainReflections : public VocalModule
{
public:
    PorcelainReflections();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    juce::String getName() const override { return "Porcelain Reflections"; }

    void setTileScatter(float amount) { tileScatter = juce::jlimit(0.0f, 1.0f, amount); }
    void setEdgeSlap(float amount) { edgeSlap = juce::jlimit(0.0f, 1.0f, amount); }

private:
    float tileScatter = 0.0f;
    float edgeSlap = 0.0f;

    // Multiple delay lines for reflections
    static constexpr int NUM_REFLECTIONS = 8;
    juce::dsp::DelayLine<float> reflections[NUM_REFLECTIONS];
    juce::Random random;
};

//==============================================================================
// 7. Steam Modulator
// Humidity-based sonic warping
class SteamModulator : public VocalModule
{
public:
    SteamModulator();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    juce::String getName() const override { return "Steam Modulator"; }

    void setHumidity(float amount) { humidity = juce::jlimit(0.0f, 1.0f, amount); }
    void setFogMode(bool enabled) { fogMode = enabled; }

private:
    float humidity = 0.0f;
    bool fogMode = false;

    juce::dsp::IIR::Filter<float> highFreqDamper;
    juce::Random random;
    float steamIntensity = 0.0f;
};

//==============================================================================
// MODULE CATEGORY 3: Funny / Chaotic / Character Modes

//==============================================================================
// 10. Rubber Duck FM
// Formant-following FM quack
class RubberDuckFM : public VocalModule
{
public:
    RubberDuckFM();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    juce::String getName() const override { return "Rubber Duck FM"; }

    enum QuackMode {
        WetQuack,
        AngryDuck,
        SlowWobble,
        Cartoon
    };

    void setQuackMode(QuackMode mode) { quackMode = mode; }
    void setQuackIntensity(float intensity) { quackIntensity = juce::jlimit(0.0f, 1.0f, intensity); }

private:
    QuackMode quackMode = WetQuack;
    float quackIntensity = 0.0f;

    float fmPhase = 0.0f;
    juce::Random random;
};

//==============================================================================
// 11. Soap Bar Glitch
// Slippery pitch and grain effects
class SoapBarGlitch : public VocalModule
{
public:
    SoapBarGlitch();

    void prepare(double sampleRate, int samplesPerBlock) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    juce::String getName() const override { return "Soap Bar Glitch"; }

    void setSlipperiness(float amount) { slipperiness = juce::jlimit(0.0f, 1.0f, amount); }
    void setSoapyBlur(float amount) { soapyBlur = juce::jlimit(0.0f, 1.0f, amount); }

private:
    float slipperiness = 0.0f;
    float soapyBlur = 0.0f;

    juce::dsp::DelayLine<float> grainBuffer{88200};
    juce::Random random;
};

//==============================================================================
// Main Vocal Processor - orchestrates all modules
class VocalProcessor
{
public:
    VocalProcessor();
    ~VocalProcessor() = default;

    void prepare(double sampleRate, int samplesPerBlock);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    // Access modules
    PitchDriftBrain& getPitchDriftBrain() { return pitchDriftBrain; }
    FormantWhispers& getFormantWhispers() { return formantWhispers; }
    BreathNoiseEngine& getBreathNoiseEngine() { return breathNoiseEngine; }
    TimingWobble& getTimingWobble() { return timingWobble; }
    VolumePersonality& getVolumePersonality() { return volumePersonality; }
    PorcelainReflections& getPorcelainReflections() { return porcelainReflections; }
    SteamModulator& getSteamModulator() { return steamModulator; }
    RubberDuckFM& getRubberDuckFM() { return rubberDuckFM; }
    SoapBarGlitch& getSoapBarGlitch() { return soapBarGlitch; }

    // Master wet/dry
    void setMasterMix(float mix) { masterMix = juce::jlimit(0.0f, 1.0f, mix); }
    float getMasterMix() const { return masterMix; }

private:
    // Category 1: Human Vocal Randomizers
    PitchDriftBrain pitchDriftBrain;
    FormantWhispers formantWhispers;
    BreathNoiseEngine breathNoiseEngine;
    TimingWobble timingWobble;
    VolumePersonality volumePersonality;

    // Category 2: Environmental / Bathtub Randomizers
    PorcelainReflections porcelainReflections;
    SteamModulator steamModulator;

    // Category 3: Funny / Chaotic / Character Modes
    RubberDuckFM rubberDuckFM;
    SoapBarGlitch soapBarGlitch;

    float masterMix = 0.5f;
    juce::AudioBuffer<float> dryBuffer;
};

#include "VocalProcessor.h"

//==============================================================================
// PitchDriftBrain Implementation
//==============================================================================
PitchDriftBrain::PitchDriftBrain()
{
}

void PitchDriftBrain::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    numChannels = 2;  // Prepare for stereo

    // Clear existing shifters
    shifters.clear();
    inputBuffers.clear();
    outputBuffers.clear();
    outputFIFOs.clear();
    inputPos.clear();
    outputPos.clear();
    outputAvailable.clear();

    // Create one RubberBand shifter per channel
    for (int ch = 0; ch < numChannels; ++ch)
    {
        shifters.push_back(std::make_unique<RubberBand::RubberBandLiveShifter>(
            (size_t)sampleRate,
            1,  // mono per channel
            RubberBand::RubberBandLiveShifter::OptionWindowShort
        ));
    }

    rbBlockSize = shifters[0]->getBlockSize();

    // Allocate buffers for each channel
    for (int ch = 0; ch < numChannels; ++ch)
    {
        inputBuffers.push_back(std::vector<float>(rbBlockSize, 0.0f));
        outputBuffers.push_back(std::vector<float>(rbBlockSize, 0.0f));
        // FIFO needs to hold at least 2 blocks worth of samples
        outputFIFOs.push_back(std::vector<float>(rbBlockSize * 4, 0.0f));
        inputPos.push_back(0);
        outputPos.push_back(0);
        outputAvailable.push_back(0);
    }
}

void PitchDriftBrain::process(juce::AudioBuffer<float>& buffer)
{
    // Skip entirely if no range is set (both at 0)
    if (!enabled || (centsLow >= 0.0f && centsHigh <= 0.0f) || shifters.empty())
        return;

    const int numSamples = buffer.getNumSamples();
    const int bufferChannels = buffer.getNumChannels();

    // LFO frequency: lfoSpeed 0-1 maps to 0.1 Hz to 5 Hz
    float lfoFreqHz = 0.1f + lfoSpeed * 4.9f;
    float lfoIncrement = lfoFreqHz / (float)currentSampleRate;

    // Update LFO and pitch target once per buffer (block rate)
    for (int i = 0; i < numSamples; ++i)
    {
        lfoPhase += lfoIncrement;
        if (lfoPhase >= 1.0f)
            lfoPhase -= 1.0f;

        float lfoValue = std::sin(lfoPhase * juce::MathConstants<float>::twoPi);

        // Detect peaks and valleys to update targets
        bool isPositive = lfoValue >= 0.0f;
        if (isPositive != wasPositive)
        {
            // We've crossed zero - save current target as previous
            previousCents = targetCents;

            // Determine if we're heading to a peak (positive half) or valley (negative half)
            bool headingToPeak = isPositive;

            if (randomizeMode)
            {
                // Random mode: pick random target within range
                targetCents = centsLow + random.nextFloat() * (centsHigh - centsLow);
            }
            else
            {
                // High/Low mode: use exact slider values
                if (headingToPeak)
                    targetCents = centsHigh;  // Peak = sharp (high)
                else
                    targetCents = centsLow;   // Valley = flat (low)
            }

            wasPositive = isPositive;
        }

        // Use the LFO waveform to ease between previous and target
        // Map LFO from -1..+1 to 0..1 for interpolation
        float t = (lfoValue + 1.0f) * 0.5f;

        // In positive half (0.5 to 1.0), we're easing toward target
        // In negative half (0.0 to 0.5), we're also easing toward target but from other side
        // So we use absolute position within the current half-cycle
        if (wasPositive)
        {
            // In positive half: t goes from 0.5 to 1.0 (peak) and back to 0.5
            // Remap so we smoothly interpolate from previous to target
            currentCents = previousCents + (targetCents - previousCents) * t;
        }
        else
        {
            // In negative half: t goes from 0.5 to 0.0 (valley) and back to 0.5
            // Invert t so we interpolate correctly
            currentCents = previousCents + (targetCents - previousCents) * (1.0f - t);
        }
    }

    // Convert cents to pitch scale: scale = 2^(cents/1200)
    double pitchScale = std::pow(2.0, currentCents / 1200.0);

    // Process each channel independently
    for (int channel = 0; channel < bufferChannels && channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto& shifter = shifters[channel];
        auto& inputBuf = inputBuffers[channel];
        auto& outputBuf = outputBuffers[channel];
        auto& outputFIFO = outputFIFOs[channel];
        size_t& inPos = inputPos[channel];
        size_t& outPos = outputPos[channel];
        size_t& outAvail = outputAvailable[channel];

        shifter->setPitchScale(pitchScale);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Add input sample to buffer
            inputBuf[inPos] = channelData[sample];
            inPos++;

            // When input buffer is full, process with RubberBand
            if (inPos >= rbBlockSize)
            {
                const float* inPtr = inputBuf.data();
                float* outPtr = outputBuf.data();
                shifter->shift(&inPtr, &outPtr);

                // Copy output to FIFO
                size_t fifoSize = outputFIFO.size();
                size_t writePos = (outPos + outAvail) % fifoSize;
                for (size_t i = 0; i < rbBlockSize; ++i)
                {
                    outputFIFO[(writePos + i) % fifoSize] = outputBuf[i];
                }
                outAvail += rbBlockSize;

                inPos = 0;
            }

            // Read from output FIFO if samples are available
            if (outAvail > 0)
            {
                channelData[sample] = outputFIFO[outPos];
                outPos = (outPos + 1) % outputFIFO.size();
                outAvail--;
            }
            // If no output available yet (initial latency), pass through or output silence
            // We'll pass through the input to avoid silence during startup
        }
    }
}

void PitchDriftBrain::reset()
{
    for (auto& shifter : shifters)
    {
        if (shifter)
            shifter->reset();
    }

    for (auto& buf : inputBuffers)
        std::fill(buf.begin(), buf.end(), 0.0f);
    for (auto& buf : outputBuffers)
        std::fill(buf.begin(), buf.end(), 0.0f);
    for (auto& fifo : outputFIFOs)
        std::fill(fifo.begin(), fifo.end(), 0.0f);

    for (size_t i = 0; i < inputPos.size(); ++i)
    {
        inputPos[i] = 0;
        outputPos[i] = 0;
        outputAvailable[i] = 0;
    }

    lfoPhase = 0.0f;
    currentCents = 0.0f;
    targetCents = 0.0f;
    previousCents = 0.0f;
    wasPositive = true;
    wasPeak = false;
}

//==============================================================================
// FormantWhispers Implementation
//==============================================================================
FormantWhispers::FormantWhispers()
{
}

void FormantWhispers::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    numChannels = 2;  // Prepare for stereo

    // Clear existing shifters
    shifters.clear();
    inputBuffers.clear();
    outputBuffers.clear();
    outputFIFOs.clear();
    inputPos.clear();
    outputPos.clear();
    outputAvailable.clear();

    // Create one RubberBand shifter per channel
    // Using OptionFormantPreserved to allow independent formant control
    for (int ch = 0; ch < numChannels; ++ch)
    {
        shifters.push_back(std::make_unique<RubberBand::RubberBandLiveShifter>(
            (size_t)sampleRate,
            1,  // mono per channel
            RubberBand::RubberBandLiveShifter::OptionWindowShort
        ));
    }

    rbBlockSize = shifters[0]->getBlockSize();

    // Allocate buffers for each channel
    for (int ch = 0; ch < numChannels; ++ch)
    {
        inputBuffers.push_back(std::vector<float>(rbBlockSize, 0.0f));
        outputBuffers.push_back(std::vector<float>(rbBlockSize, 0.0f));
        outputFIFOs.push_back(std::vector<float>(rbBlockSize * 4, 0.0f));
        inputPos.push_back(0);
        outputPos.push_back(0);
        outputAvailable.push_back(0);
    }
}

void FormantWhispers::process(juce::AudioBuffer<float>& buffer)
{
    // Skip if no range is set or not enabled
    if (!enabled || (formantShiftLow >= 0.0f && formantShiftHigh <= 0.0f) || shifters.empty())
        return;

    const int numSamples = buffer.getNumSamples();
    const int bufferChannels = buffer.getNumChannels();

    // LFO frequency: formantLFOSpeed 0-1 maps to 0.1 Hz to 5 Hz
    float lfoFreqHz = 0.1f + formantLFOSpeed * 4.9f;
    float lfoIncrement = lfoFreqHz / (float)currentSampleRate;

    // Update LFO and formant target
    for (int i = 0; i < numSamples; ++i)
    {
        formantLFOPhase += lfoIncrement;
        if (formantLFOPhase >= 1.0f)
            formantLFOPhase -= 1.0f;

        float lfoValue = std::sin(formantLFOPhase * juce::MathConstants<float>::twoPi);

        // Detect zero crossings to update targets
        bool isPositive = lfoValue >= 0.0f;
        if (isPositive != formantWasPositive)
        {
            previousFormantShift = targetFormantShift;
            bool headingToPeak = isPositive;

            if (formantRandomizeMode)
            {
                targetFormantShift = formantShiftLow + random.nextFloat() * (formantShiftHigh - formantShiftLow);
            }
            else
            {
                if (headingToPeak)
                    targetFormantShift = formantShiftHigh;
                else
                    targetFormantShift = formantShiftLow;
            }

            formantWasPositive = isPositive;
        }

        // Ease between previous and target using LFO position
        float t = (lfoValue + 1.0f) * 0.5f;
        if (formantWasPositive)
            currentFormantShift = previousFormantShift + (targetFormantShift - previousFormantShift) * t;
        else
            currentFormantShift = previousFormantShift + (targetFormantShift - previousFormantShift) * (1.0f - t);
    }

    // Convert formant shift to scale
    // formantShift of -1 = formants shifted down (bigger character)
    // formantShift of +1 = formants shifted up (smaller character)
    // We use setFormantScale - values > 1 shift formants up, < 1 shift down
    // Map -1..+1 to 0.5..2.0
    double formantScale = std::pow(2.0, currentFormantShift);

    // Process each channel independently
    for (int channel = 0; channel < bufferChannels && channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto& shifter = shifters[channel];
        auto& inputBuf = inputBuffers[channel];
        auto& outputBuf = outputBuffers[channel];
        auto& outputFIFO = outputFIFOs[channel];
        size_t& inPos = inputPos[channel];
        size_t& outPos = outputPos[channel];
        size_t& outAvail = outputAvailable[channel];

        // Set pitch to 1.0 (no pitch change) but shift formants
        shifter->setPitchScale(1.0);
        shifter->setFormantScale(formantScale);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Add input sample to buffer
            inputBuf[inPos] = channelData[sample];
            inPos++;

            // When input buffer is full, process with RubberBand
            if (inPos >= rbBlockSize)
            {
                const float* inPtr = inputBuf.data();
                float* outPtr = outputBuf.data();
                shifter->shift(&inPtr, &outPtr);

                // Copy output to FIFO
                size_t fifoSize = outputFIFO.size();
                size_t writePos = (outPos + outAvail) % fifoSize;
                for (size_t i = 0; i < rbBlockSize; ++i)
                {
                    outputFIFO[(writePos + i) % fifoSize] = outputBuf[i];
                }
                outAvail += rbBlockSize;

                inPos = 0;
            }

            // Read from output FIFO if samples are available
            if (outAvail > 0)
            {
                channelData[sample] = outputFIFO[outPos];
                outPos = (outPos + 1) % outputFIFO.size();
                outAvail--;
            }
        }
    }
}

void FormantWhispers::reset()
{
    for (auto& shifter : shifters)
    {
        if (shifter)
            shifter->reset();
    }

    for (auto& buf : inputBuffers)
        std::fill(buf.begin(), buf.end(), 0.0f);
    for (auto& buf : outputBuffers)
        std::fill(buf.begin(), buf.end(), 0.0f);
    for (auto& fifo : outputFIFOs)
        std::fill(fifo.begin(), fifo.end(), 0.0f);

    for (size_t i = 0; i < inputPos.size(); ++i)
    {
        inputPos[i] = 0;
        outputPos[i] = 0;
        outputAvailable[i] = 0;
    }

    formantLFOPhase = 0.0f;
    formantWasPositive = true;
    currentFormantShift = 0.0f;
    targetFormantShift = 0.0f;
    previousFormantShift = 0.0f;
}

//==============================================================================
// BreathNoiseEngine Implementation
//==============================================================================
BreathNoiseEngine::BreathNoiseEngine()
{
}

void BreathNoiseEngine::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    // Prepare breath noise filter (high-pass for breath-like noise)
    breathFilter.prepare({sampleRate, (juce::uint32)samplesPerBlock, 1});
    breathFilter.reset();
}

void BreathNoiseEngine::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled || mix <= 0.0f || breathIntensity <= 0.0f)
        return;

    const int numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Envelope follower
            float input = channelData[sample];
            float env = std::abs(input);
            envelopeFollower = envelopeFollower * 0.999f + env * 0.001f;

            // Generate breath noise based on envelope
            if (envelopeFollower > 0.01f)
            {
                float noise = (random.nextFloat() * 2.0f - 1.0f) * breathIntensity * 0.1f;
                channelData[sample] += noise * envelopeFollower;
            }
        }
    }
}

void BreathNoiseEngine::reset()
{
    breathFilter.reset();
    envelopeFollower = 0.0f;
}

//==============================================================================
// TimingWobble Implementation
//==============================================================================
TimingWobble::TimingWobble()
{
}

void TimingWobble::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    timingBuffer.prepare({sampleRate, (juce::uint32)samplesPerBlock, 1});
    timingBuffer.reset();
}

void TimingWobble::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled || wobbleAmount <= 0.0f)
        return;

    const int numSamples = buffer.getNumSamples();

    // Maximum wobble in samples (at 44.1kHz: ~10ms max delay)
    float maxWobbleSamples = (float)currentSampleRate * 0.01f * wobbleAmount;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = channelData[sample];

            // Detect transients (simple envelope follower)
            float env = std::abs(input);

            // Slowly modulate target delay with swing feel
            if (sample % 256 == 0)
            {
                // Random micro-timing drift
                float wobble = (random.nextFloat() - 0.5f) * 2.0f * maxWobbleSamples;

                // Add swing feel (slight delay on even beats)
                float swing = swingFeel * maxWobbleSamples * 0.5f;

                float targetDelay = juce::jmax(0.0f, wobble + swing);
                currentDelay = currentDelay * 0.95f + targetDelay * 0.05f;
            }

            // Push to delay line
            timingBuffer.pushSample(0, input);

            // Pop with variable delay
            float delayedSample = timingBuffer.popSample(0, currentDelay);

            // Mix
            channelData[sample] = input * (1.0f - mix) + delayedSample * mix;
        }
    }
}

void TimingWobble::reset()
{
    timingBuffer.reset();
    currentDelay = 0.0f;
}

//==============================================================================
// VolumePersonality Implementation
//==============================================================================
VolumePersonality::VolumePersonality()
{
}

void VolumePersonality::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
}

void VolumePersonality::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled || intensity <= 0.0f)
        return;

    // Simple volume wobble based on personality
    const int numSamples = buffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Slowly move toward target gain
        currentGain = currentGain * 0.999f + targetGain * 0.001f;

        // Randomly adjust target every N samples
        if (sample % 512 == 0)
        {
            float wobble = (random.nextFloat() * 2.0f - 1.0f) * intensity * 0.1f;
            targetGain = 1.0f + wobble;
        }

        // Apply gain
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            buffer.setSample(channel, sample,
                           buffer.getSample(channel, sample) * currentGain);
        }
    }
}

void VolumePersonality::reset()
{
    currentGain = 1.0f;
    targetGain = 1.0f;
}

//==============================================================================
// PorcelainReflections Implementation
//==============================================================================
PorcelainReflections::PorcelainReflections()
{
}

void PorcelainReflections::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    juce::dsp::ProcessSpec spec{sampleRate, (juce::uint32)samplesPerBlock, 1};
    for (int i = 0; i < NUM_REFLECTIONS; ++i)
    {
        reflections[i].prepare(spec);
        reflections[i].reset();

        // Set different delay times for each reflection (early reflections pattern)
        float delayMs = 5.0f + (i * 7.3f); // Irregular spacing
        reflections[i].setMaximumDelayInSamples((int)(sampleRate * delayMs / 1000.0));
    }
}

void PorcelainReflections::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled || tileScatter <= 0.0f)
        return;

    const int numSamples = buffer.getNumSamples();

    // Delay times in samples for different "surfaces" (tile, mirror, sink, etc.)
    const float delayTimesMs[NUM_REFLECTIONS] = {5.3f, 8.7f, 12.1f, 17.4f, 23.8f, 31.2f, 42.5f, 56.7f};
    const float gains[NUM_REFLECTIONS] = {0.3f, 0.25f, 0.2f, 0.18f, 0.15f, 0.12f, 0.1f, 0.08f};

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = channelData[sample];
            float wetSignal = 0.0f;

            // Sum all reflections
            for (int r = 0; r < NUM_REFLECTIONS; ++r)
            {
                // Add slight random modulation to delay time (simulates moving head/room chaos)
                float modulatedDelay = delayTimesMs[r] + (random.nextFloat() - 0.5f) * tileScatter * 2.0f;
                float delaySamples = (modulatedDelay / 1000.0f) * (float)currentSampleRate;

                reflections[r].pushSample(0, input);
                float delayed = reflections[r].popSample(0, delaySamples);

                // Apply gain with edge slap resonance
                float resonance = 1.0f + edgeSlap * 0.5f * std::sin((float)r * 0.8f);
                wetSignal += delayed * gains[r] * resonance;
            }

            // Mix wet with dry
            channelData[sample] = input * (1.0f - mix) + (input + wetSignal * tileScatter) * mix;
        }
    }
}

void PorcelainReflections::reset()
{
    for (int i = 0; i < NUM_REFLECTIONS; ++i)
        reflections[i].reset();
}

//==============================================================================
// SteamModulator Implementation
//==============================================================================
SteamModulator::SteamModulator()
{
}

void SteamModulator::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    highFreqDamper.prepare({sampleRate, (juce::uint32)samplesPerBlock, 1});
    highFreqDamper.reset();
}

void SteamModulator::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled || humidity <= 0.0f)
        return;

    const int numSamples = buffer.getNumSamples();

    // Calculate lowpass cutoff based on humidity (more humidity = more HF damping)
    float cutoffHz = 20000.0f - (humidity * 15000.0f); // 20kHz down to 5kHz
    cutoffHz = juce::jmax(500.0f, cutoffHz);

    // Update filter coefficients
    auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, cutoffHz, 0.707f);
    *highFreqDamper.coefficients = *coefficients;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = channelData[sample];

            // Apply humidity-based lowpass filter
            float filtered = highFreqDamper.processSample(input);

            // Slowly modulate steam intensity (gradual fog buildup)
            float targetSteam = humidity;
            steamIntensity = steamIntensity * 0.9999f + targetSteam * 0.0001f;

            // Add subtle noise layer for fog mode
            float fogNoise = 0.0f;
            if (fogMode && steamIntensity > 0.3f)
            {
                fogNoise = (random.nextFloat() * 2.0f - 1.0f) * 0.01f * steamIntensity;
            }

            // Mix: more humidity = more filtered signal
            float wetAmount = steamIntensity * mix;
            channelData[sample] = input * (1.0f - wetAmount) + (filtered + fogNoise) * wetAmount;
        }
    }
}

void SteamModulator::reset()
{
    highFreqDamper.reset();
    steamIntensity = 0.0f;
}

//==============================================================================
// RubberDuckFM Implementation
//==============================================================================
RubberDuckFM::RubberDuckFM()
{
}

void RubberDuckFM::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
}

void RubberDuckFM::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled || quackIntensity <= 0.0f)
        return;

    // Quack mode: formant-following FM synthesis
    const int numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Simple FM wobble
            float modulator = std::sin(fmPhase) * quackIntensity;
            fmPhase += (800.0f / (float)currentSampleRate) * juce::MathConstants<float>::twoPi;

            if (fmPhase > juce::MathConstants<float>::twoPi)
                fmPhase -= juce::MathConstants<float>::twoPi;

            // Mix original with FM-modulated version
            float original = channelData[sample];
            float modulated = original * (1.0f + modulator * 0.3f);
            channelData[sample] = original * (1.0f - mix) + modulated * mix;
        }
    }
}

void RubberDuckFM::reset()
{
    fmPhase = 0.0f;
}

//==============================================================================
// SoapBarGlitch Implementation
//==============================================================================
SoapBarGlitch::SoapBarGlitch()
{
}

void SoapBarGlitch::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    grainBuffer.prepare({sampleRate, (juce::uint32)samplesPerBlock, 1});
    grainBuffer.reset();
}

void SoapBarGlitch::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled || slipperiness <= 0.0f)
        return;

    const int numSamples = buffer.getNumSamples();

    // Grain parameters
    static float grainPhase = 0.0f;
    static float slipAmount = 0.0f;
    static float targetSlip = 0.0f;
    static int grainSize = 512;
    static int grainCounter = 0;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float input = channelData[sample];

            // Push to grain buffer
            grainBuffer.pushSample(0, input);

            // Random "slip" events - like soap slipping from hands
            if (random.nextFloat() < slipperiness * 0.001f)
            {
                // Trigger a slip!
                targetSlip = (random.nextFloat() * 2.0f - 1.0f) * 1000.0f; // Up to 1000 samples
                grainSize = 128 + random.nextInt(512);
            }

            // Smooth the slip amount
            slipAmount = slipAmount * 0.999f + targetSlip * 0.001f;

            // Gradually return to normal
            targetSlip *= 0.995f;

            // Calculate grain readback position
            float readPos = std::abs(slipAmount) + 1.0f;

            // Pop from grain buffer with variable delay (creates pitch shifting effect)
            float grainSample = grainBuffer.popSample(0, readPos);

            // Apply soapy blur (crossfade smear)
            if (soapyBlur > 0.0f)
            {
                // Average with neighboring positions for blur
                float blurSample1 = grainBuffer.popSample(0, readPos + 2.0f);
                float blurSample2 = grainBuffer.popSample(0, readPos + 4.0f);
                grainSample = grainSample * (1.0f - soapyBlur * 0.5f) +
                             (blurSample1 + blurSample2) * soapyBlur * 0.25f;
            }

            // Apply window function for smooth grains
            float window = 0.5f * (1.0f - std::cos(grainPhase * juce::MathConstants<float>::twoPi));
            grainPhase += 1.0f / (float)grainSize;
            if (grainPhase >= 1.0f)
            {
                grainPhase -= 1.0f;
                grainSize = 256 + random.nextInt(512);
            }

            // Mix
            float wetSample = grainSample * (0.5f + window * 0.5f);
            channelData[sample] = input * (1.0f - mix) + wetSample * mix;
        }
    }
}

void SoapBarGlitch::reset()
{
    grainBuffer.reset();
}

//==============================================================================
// VocalProcessor Implementation
//==============================================================================
VocalProcessor::VocalProcessor()
{
}

void VocalProcessor::prepare(double sampleRate, int samplesPerBlock)
{
    // Prepare all modules
    pitchDriftBrain.prepare(sampleRate, samplesPerBlock);
    formantWhispers.prepare(sampleRate, samplesPerBlock);
    breathNoiseEngine.prepare(sampleRate, samplesPerBlock);
    timingWobble.prepare(sampleRate, samplesPerBlock);
    volumePersonality.prepare(sampleRate, samplesPerBlock);
    porcelainReflections.prepare(sampleRate, samplesPerBlock);
    steamModulator.prepare(sampleRate, samplesPerBlock);
    rubberDuckFM.prepare(sampleRate, samplesPerBlock);
    soapBarGlitch.prepare(sampleRate, samplesPerBlock);

    // Allocate dry buffer for wet/dry mixing
    dryBuffer.setSize(2, samplesPerBlock);
}

void VocalProcessor::process(juce::AudioBuffer<float>& buffer)
{
    // Store dry signal
    dryBuffer.makeCopyOf(buffer, true);

    // Process through all enabled modules in sequence
    // Category 1: Human Vocal Randomizers
    pitchDriftBrain.process(buffer);
    formantWhispers.process(buffer);
    breathNoiseEngine.process(buffer);
    timingWobble.process(buffer);
    volumePersonality.process(buffer);

    // Category 2: Environmental / Bathtub
    porcelainReflections.process(buffer);
    steamModulator.process(buffer);

    // Category 3: Character Modes
    rubberDuckFM.process(buffer);
    soapBarGlitch.process(buffer);

    // Master wet/dry mix
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* wetData = buffer.getWritePointer(channel);
        const auto* dryData = dryBuffer.getReadPointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            wetData[sample] = dryData[sample] * (1.0f - masterMix) + wetData[sample] * masterMix;
        }
    }
}

void VocalProcessor::reset()
{
    pitchDriftBrain.reset();
    formantWhispers.reset();
    breathNoiseEngine.reset();
    timingWobble.reset();
    volumePersonality.reset();
    porcelainReflections.reset();
    steamModulator.reset();
    rubberDuckFM.reset();
    soapBarGlitch.reset();
}

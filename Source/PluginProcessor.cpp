#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SchlomosBathAudioProcessor::SchlomosBathAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SchlomosBathAudioProcessor::~SchlomosBathAudioProcessor()
{
}

//==============================================================================
const juce::String SchlomosBathAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SchlomosBathAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SchlomosBathAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SchlomosBathAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SchlomosBathAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SchlomosBathAudioProcessor::getNumPrograms()
{
    return 1;
}

int SchlomosBathAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SchlomosBathAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SchlomosBathAudioProcessor::getProgramName (int index)
{
    return {};
}

void SchlomosBathAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SchlomosBathAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    vocalProcessor.prepare(sampleRate, samplesPerBlock);
}

void SchlomosBathAudioProcessor::releaseResources()
{
    vocalProcessor.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SchlomosBathAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // Stereo in/out
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Input and output must match
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SchlomosBathAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't have input
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Process through vocal processor
    vocalProcessor.process(buffer);
}

//==============================================================================
bool SchlomosBathAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SchlomosBathAudioProcessor::createEditor()
{
    return new SchlomosBathAudioProcessorEditor (*this);
}

//==============================================================================
void SchlomosBathAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // TODO: Save plugin state
}

void SchlomosBathAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // TODO: Restore plugin state
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SchlomosBathAudioProcessor();
}

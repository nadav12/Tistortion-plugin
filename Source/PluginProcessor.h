/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/

struct ChainSettings
{
    float lowCutFreq { 0 };
    float highCutFreq { 0 };
};

ChainSettings getChainSettings (juce::AudioProcessorValueTreeState& apvts);

class TistortionAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TistortionAudioProcessor();
    ~TistortionAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameters()};
private:
    
    enum ChainPositions
    {
        LowCut,
        HighCut
    };
    
    
    void updateLowCutFilter (const ChainSettings& chainSettings);
    void updateHighCutFilter (const ChainSettings& chainSettings);
    void updateFilters();

    void reset() override;
    using Filter = juce::dsp::IIR::Filter<float>;
    
    using MonoChain = juce::dsp::ProcessorChain<Filter, Filter>;
    MonoChain leftChain, rightChain;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TistortionAudioProcessor)
};

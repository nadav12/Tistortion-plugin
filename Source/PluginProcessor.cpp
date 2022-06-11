/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TistortionAudioProcessor::TistortionAudioProcessor()
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

TistortionAudioProcessor::~TistortionAudioProcessor()
{
}

//==============================================================================
const juce::String TistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void TistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}




//==============================================================================
void TistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    updateFilters();
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    

}
ChainSettings getChainSettings (juce::AudioProcessorValueTreeState& apvts)
{
    ChainSettings settings;
    
    settings.lowCutFreq = apvts.getRawParameterValue ("LOWCUT")->load();
    settings.highCutFreq = apvts.getRawParameterValue("HIGHCUT")->load();

    return settings;
}

void TistortionAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void TistortionAudioProcessor::updateLowCutFilter (const ChainSettings& chainSettings)
{
    auto lowCutCoefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass (getSampleRate(),
                                                                                 chainSettings.lowCutFreq
                                                                                        );
        
    *leftChain.get<ChainPositions::LowCut>().coefficients = *lowCutCoefficients;
    *rightChain.get<ChainPositions::LowCut>().coefficients = *lowCutCoefficients;
}
void TistortionAudioProcessor::updateHighCutFilter (const ChainSettings& chainSettings)
{
    auto highCutCoefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(getSampleRate(), chainSettings.highCutFreq);
    *leftChain.get<ChainPositions::HighCut>().coefficients = *highCutCoefficients;
    *rightChain.get<ChainPositions::HighCut>().coefficients = *highCutCoefficients;

}
void TistortionAudioProcessor::updateFilters()
{
    auto chainSettings = getChainSettings (apvts);
    updateHighCutFilter(chainSettings);
    updateLowCutFilter (chainSettings);
}

void TistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    auto drive = apvts.getRawParameterValue("DRIVE");
    auto range = apvts.getRawParameterValue("RANGE");
    auto volume = apvts.getRawParameterValue("VOLUME");
    auto curve = apvts.getRawParameterValue("CURVE");
  
    float driver = drive->load();
    float ranger = range->load();
    float volumer = volume->load();
    float curver = curve->load();

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear (i, 0, buffer.getNumSamples());
    updateFilters();

    juce::dsp::AudioBlock<float> block (buffer);
    
    auto leftBlock = block.getSingleChannelBlock (0);
    auto rightBlock = block.getSingleChannelBlock (1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext (leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext (rightBlock);
    
    leftChain.process (leftContext);
    rightChain.process (rightContext);
    
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for(int sample = 0; sample < buffer.getNumSamples(); sample ++)
        {

            channelData[sample] *= (driver * ranger);

            auto drivenSignal = 2.0f / M_PI * atan(M_PI/curver * channelData[sample]);
            channelData[sample] = drivenSignal * 0.5f * volumer;

        }
        
   }

}

//==============================================================================
bool TistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TistortionAudioProcessor::createEditor()
{
    return new TistortionAudioProcessorEditor (*this);
}

//==============================================================================
void TistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    DBG("getstate");
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void TistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    DBG("setstate");
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));


}

void TistortionAudioProcessor::reset()
{
    rightChain.reset();
    leftChain.reset();
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TistortionAudioProcessor();
}



juce::AudioProcessorValueTreeState::ParameterLayout TistortionAudioProcessor:: createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DRIVE", "Drive", 0.f, 1.f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RANGE", "Range", 1.f, 2000.f, 3.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("VOLUME", "Volume", 0.f, 1.f, 0.999f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("LOWCUT", "LowCut", 20.f, 1400.f, 250.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("HIGHCUT", "HighCut", 2000.f, 20000.f, 18000.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("CURVE", "Curve", 0.1f, 2.f, 1.5f));
    return {params.begin(), params.end()};
}



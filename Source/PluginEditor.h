/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "KnobDesign.h"
#include "TSlider.h"



//==============================================================================
/**
*/
class TistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TistortionAudioProcessorEditor (TistortionAudioProcessor&);
    ~TistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
private:
    KnobDesign knobDesign;

    juce::Image pluginBG;
    TSlider driveSlider, rangeSlider, volumeSlider, cutOffSlider, highCutSlider;
    juce::Label driveLabel, rangeLabel, volumeLabel, cutOffLabel, highCutLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveSliderAttachment,rangeSliderAttachment,blendSliderAttachment,volumeSliderAttachment,cutOffSliderAttachment,highCutSliderAttachment;

    

    TistortionAudioProcessor& audioProcessor;  

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TistortionAudioProcessorEditor)
};

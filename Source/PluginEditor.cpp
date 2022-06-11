/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "KnobDesign.h"
#include "TSlider.h"

//==============================================================================
TistortionAudioProcessorEditor::TistortionAudioProcessorEditor (TistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    //drive

    driveSlider.setLookAndFeel(&knobDesign);
    addAndMakeVisible(driveSlider);
    
    addAndMakeVisible (driveLabel);
    driveLabel.setText ("Drive", juce::dontSendNotification);
    driveLabel.attachToComponent (&driveSlider, false);
    


    driveSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DRIVE", driveSlider);
//    driveSliderAttachment.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (audioProcessor.apvts, "DRIVE", driveSlider));
    //===============================================================================================================
    //range

    rangeSlider.setLookAndFeel(&knobDesign);
    addAndMakeVisible(rangeSlider);
    
    addAndMakeVisible (rangeLabel);
    rangeLabel.setText ("Range", juce::dontSendNotification);
    rangeLabel.attachToComponent (&rangeSlider, false);

    
    rangeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "RANGE", rangeSlider);
    //===============================================================================================================

    //===============================================================================================================
    //volume

    volumeSlider.setLookAndFeel(&knobDesign);

    addAndMakeVisible(volumeSlider);
    
    addAndMakeVisible (volumeLabel);
    volumeLabel.setText ("Volume", juce::dontSendNotification);
    volumeLabel.attachToComponent (&volumeSlider, false);

    volumeSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "VOLUME", volumeSlider);

     //===============================================================================================================
    //Cut Off

    cutOffSlider.setLookAndFeel(&knobDesign);

    addAndMakeVisible(cutOffSlider);
    
    addAndMakeVisible (cutOffLabel);
    cutOffLabel.setText ("Low Cut", juce::dontSendNotification);
    cutOffLabel.attachToComponent (&cutOffSlider, false);
    
    cutOffSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LOWCUT", cutOffSlider);
    cutOffSlider.setSkewFactorFromMidPoint(1200.0);
 
    
    //===============================================================================================================
   //Cut Off

//   highCutSlider.setLookAndFeel(&knobDesign);
//
//   addAndMakeVisible(highCutSlider);
//
//   addAndMakeVisible (highCutLabel);
//    highCutLabel.setText ("High Cut", juce::dontSendNotification);
//    highCutLabel.attachToComponent (&highCutSlider, false);
//    highCutLabel.setJustificationType(juce::Justification::centredBottom);
//
//   highCutSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HIGHCUT", highCutSlider);
//   highCutSlider.setSkewFactorFromMidPoint(1200.0);

     //===============================================================================================================
    curveSlider.setLookAndFeel(&knobDesign);
    addAndMakeVisible(curveSlider);
    addAndMakeVisible (curveLabel);
    curveLabel.setText ("Curve", juce::dontSendNotification);
    curveLabel.attachToComponent (&curveSlider, false);
    
    curveSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "CURVE", curveSlider);
//    setResizable(true, true);
//
//    setResizeLimits(400, 400, 800, 1000);
    setSize (350, 500);
    
}

TistortionAudioProcessorEditor::~TistortionAudioProcessorEditor()
{
}

//==============================================================================
void TistortionAudioProcessorEditor::paint (juce::Graphics& g)
{

    
    pluginBG = juce::ImageCache::getFromMemory(BinaryData::bg_png,BinaryData::bg_pngSize);
    g.drawImageWithin(pluginBG, 0, 0, AudioProcessorEditor::getWidth(), AudioProcessorEditor::getHeight(), juce::RectanglePlacement::stretchToFit);
//    g.fillAll (juce::Colours::tomato);

    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Tistortion by Twina", getLocalBounds(), juce::Justification::centredTop, 1);
    
}

void TistortionAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds().removeFromBottom(325);
    bounds = bounds.removeFromTop(300);
    int knobHeight = 120;
    int knobWidth = 120;
    
//    int knobHeight = jlimit(100, 160, getParentHeight()/5);
//    int knobWidth = jlimit(100, 160, getParentWidth()/5);

    juce::FlexBox flexbox;
    flexbox.flexDirection = juce::FlexBox::Direction::row;
    flexbox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexbox.justifyContent = juce::FlexBox::JustifyContent::center;
    flexbox.alignContent = juce::FlexBox::AlignContent::spaceAround;

    flexbox.items.add(juce::FlexItem(knobHeight, knobWidth, driveSlider));
    flexbox.items.add(juce::FlexItem(knobHeight, knobWidth, rangeSlider));
    flexbox.items.add(juce::FlexItem(knobHeight, knobWidth, volumeSlider));
    flexbox.items.add(juce::FlexItem(knobHeight, knobWidth, cutOffSlider));
    flexbox.items.add(juce::FlexItem(knobHeight, knobWidth, curveSlider));
//    flexbox.items.add(juce::FlexItem(knobHeight, knobWidth, highCutSlider));

    flexbox.performLayout(bounds);

}

/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class NeedVSToWorkPlsAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NeedVSToWorkPlsAudioProcessorEditor (NeedVSToWorkPlsAudioProcessor&, juce::AudioProcessorValueTreeState& apvts);
    ~NeedVSToWorkPlsAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
   
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NeedVSToWorkPlsAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;
    juce::Slider dwSlider;
    std::unique_ptr<SliderAttachment> dwAttachment;
    juce::Slider fbSlider;
    std::unique_ptr<SliderAttachment> fbAttachment;
    juce::Slider timeSlider;
    std::unique_ptr<SliderAttachment> timeAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeedVSToWorkPlsAudioProcessorEditor)
};

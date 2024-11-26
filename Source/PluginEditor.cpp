/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <format>

//==============================================================================

NeedVSToWorkPlsAudioProcessorEditor::NeedVSToWorkPlsAudioProcessorEditor (NeedVSToWorkPlsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    sliderObject.setSliderStyle(juce::Slider::LinearBarVertical);
    sliderObject.setRange(0, 100, 0);
    sliderObject.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderObject.setPopupDisplayEnabled(true, true, this);
    sliderObject.setTextValueSuffix(" Minou Slider");
    sliderObject.setValue(0);

    addAndMakeVisible(&sliderObject);
}

NeedVSToWorkPlsAudioProcessorEditor::~NeedVSToWorkPlsAudioProcessorEditor()
{
}

//==============================================================================
void NeedVSToWorkPlsAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::white);

    g.setColour (juce::Colours::black);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText("Meow", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void NeedVSToWorkPlsAudioProcessorEditor::resized()
{
    sliderObject.setBounds(40, 30, 20, getHeight() - 60);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

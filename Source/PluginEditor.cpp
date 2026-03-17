/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <format>

//==============================================================================

NeedVSToWorkPlsAudioProcessorEditor::NeedVSToWorkPlsAudioProcessorEditor(NeedVSToWorkPlsAudioProcessor& p, juce::AudioProcessorValueTreeState& apvts)
    : AudioProcessorEditor(&p), audioProcessor(p), apvts(apvts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);

    juce::Slider dwSlider;
    juce::Slider fbSlider;
    juce::Slider timeSlider;
    startTimerHz(15); // 30 FPS UI refresh
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

    drawBuffer(g);
    float writeX = (float)audioProcessor.delay.dWritePtr / audioProcessor.delay.dSpl * getWidth();
    float readX = (float)audioProcessor.delay.dReadPtr / audioProcessor.delay.dSpl * getWidth();
    g.setColour(juce::Colours::red);
    g.drawLine(writeX, 0, writeX, getHeight(), 2);

    g.setColour(juce::Colours::blue);
    g.drawLine(readX, 0, readX, getHeight(), 2);

}

void NeedVSToWorkPlsAudioProcessorEditor::timerCallback()
{
    repaint(getLocalBounds());
}

void NeedVSToWorkPlsAudioProcessorEditor::drawBuffer(juce::Graphics& g)
{
    auto& buffer = audioProcessor.delay.dBuffer;

    if (buffer.getNumSamples() == 0)
        return;

    int w = getWidth();
    int h = getHeight();

    auto* data = buffer.getReadPointer(0);
    int size = buffer.getNumSamples();

    int step = juce::jmax(1, size / w);

    juce::Path path;
    path.startNewSubPath(0, h / 2);

    for (int i = 0; i < size; i += step)
    {
        float x = (float)i / size * w;
        float y = h / 2 - data[i] * (h / 2);
        path.lineTo(x, y);
    }

    g.setColour(juce::Colours::lime);
    g.strokePath(path, juce::PathStrokeType(2.0f));
}

void NeedVSToWorkPlsAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

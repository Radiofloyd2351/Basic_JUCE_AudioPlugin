#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

class NeedVSToWorkPlsAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
	NeedVSToWorkPlsAudioProcessorEditor(NeedVSToWorkPlsAudioProcessor&, juce::AudioProcessorValueTreeState& apvts);
	~NeedVSToWorkPlsAudioProcessorEditor() override;

	void paint(juce::Graphics&) override;
	void drawBuffer(juce::Graphics& g);
	void resized() override;
	void timerCallback() override;

private:
	NeedVSToWorkPlsAudioProcessor& audioProcessor;
	juce::AudioProcessorValueTreeState& apvts;

	// Sliders
	juce::Slider dwSlider;
	juce::Slider fbSlider;
	juce::Slider timeSlider;

	// Labels
	juce::Label dwLabel;
	juce::Label fbLabel;
	juce::Label timeLabel;

	// Attachments
	std::unique_ptr<SliderAttachment> dwAttachment;
	std::unique_ptr<SliderAttachment> fbAttachment;
	std::unique_ptr<SliderAttachment> timeAttachment;

	juce::ToggleButton clearButton;
	juce::ToggleButton reverseButton;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> clearAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverseAttachment;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NeedVSToWorkPlsAudioProcessorEditor)
};
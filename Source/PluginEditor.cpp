#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

NeedVSToWorkPlsAudioProcessorEditor::NeedVSToWorkPlsAudioProcessorEditor(
	NeedVSToWorkPlsAudioProcessor& p,
	juce::AudioProcessorValueTreeState& apvts)
	: AudioProcessorEditor(&p), audioProcessor(p), apvts(apvts)
{
	setSize(320, 420);

	auto setupSlider = [](juce::Slider& s)
		{
			s.setSliderStyle(juce::Slider::LinearHorizontal);
			s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
		};

	auto setupLabel = [](juce::Label& l, const juce::String& text)
		{
			l.setText(text, juce::dontSendNotification);
			l.setJustificationType(juce::Justification::centredLeft);
		};

	// --- Dry/Wet ---
	setupLabel(dwLabel, "Dry / Wet");
	setupSlider(dwSlider);
	addAndMakeVisible(dwLabel);
	addAndMakeVisible(dwSlider);
	dwAttachment = std::make_unique<SliderAttachment>(apvts, "DW", dwSlider);

	// --- Feedback ---
	setupLabel(fbLabel, "Feedback");
	setupSlider(fbSlider);
	addAndMakeVisible(fbLabel);
	addAndMakeVisible(fbSlider);
	fbAttachment = std::make_unique<SliderAttachment>(apvts, "FB", fbSlider);

	// --- Time ---
	setupLabel(timeLabel, "Time");
	setupSlider(timeSlider);
	addAndMakeVisible(timeLabel);
	addAndMakeVisible(timeSlider);
	timeAttachment = std::make_unique<SliderAttachment>(apvts, "TIME", timeSlider);

	// --- Clear Button ---
	clearButton.setButtonText("Clear");
	addAndMakeVisible(clearButton);
	clearAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "CLEAR", clearButton);

	// --- Reverse Button ---
	reverseButton.setButtonText("Reverse");
	addAndMakeVisible(reverseButton);
	reverseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "REVERSE", reverseButton);

	startTimerHz(15);
}

NeedVSToWorkPlsAudioProcessorEditor::~NeedVSToWorkPlsAudioProcessorEditor()
{
}

//==============================================================================

void NeedVSToWorkPlsAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colour(30, 30, 30)); // dark background

	g.setColour(juce::Colours::black);
	g.setFont(15.0f);
	g.drawFittedText("Meow", 0, 0, getWidth(), 30, juce::Justification::centred, 1);

	drawBuffer(g);

	float writeX = (float)audioProcessor.delay.dWritePtr / audioProcessor.delay.dSpl * getWidth();
	float readX = (float)audioProcessor.delay.dReadPtr / audioProcessor.delay.dSpl * getWidth();

	g.setColour(juce::Colours::red);
	g.drawLine(writeX, 0, writeX, getHeight(), 2);

	g.setColour(juce::Colours::blue);
	g.drawLine(readX, 0, readX, getHeight(), 2);
}

//==============================================================================

void NeedVSToWorkPlsAudioProcessorEditor::timerCallback()
{
	repaint();
}

//==============================================================================

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

//==============================================================================

void NeedVSToWorkPlsAudioProcessorEditor::resized()
{
	auto area = getLocalBounds().reduced(15);

	int rowHeight = 40;
	int labelWidth = 100;
	int spacing = 10;

	auto placeSlider = [&](juce::Label& label, juce::Slider& slider)
		{
			auto row = area.removeFromTop(rowHeight);

			label.setBounds(row.removeFromLeft(labelWidth));
			slider.setBounds(row);

			area.removeFromTop(spacing);
		};

	placeSlider(dwLabel, dwSlider);
	placeSlider(fbLabel, fbSlider);
	placeSlider(timeLabel, timeSlider);

	// Buttons row
	auto buttonRow = area.removeFromTop(rowHeight);

	clearButton.setBounds(buttonRow.removeFromLeft(buttonRow.getWidth() / 2).reduced(5));
	reverseButton.setBounds(buttonRow.reduced(5));
}
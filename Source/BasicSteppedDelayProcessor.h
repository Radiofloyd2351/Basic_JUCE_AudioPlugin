#pragma once
#include "AbsDelayProcessor.h"
class BasicSteppedDelayProcessor : public AbsDelayProcessor
{
public:
	BasicSteppedDelayProcessor();
	void init(int channels, int sampleRate, int delaySamples, int delayTimeSamples) override;
	juce::AudioBuffer<float> writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer) override;
	void performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int timeSamples);
};


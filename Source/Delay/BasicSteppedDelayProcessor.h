#pragma once
#include "AbsDelayProcessor.h"
class BasicSteppedDelayProcessor : public AbsDelayProcessor
{
public:
	BasicSteppedDelayProcessor();
	void init(int channels, int sampleRate, int delaySamples, int delayTimeSamples) override;
	juce::AudioBuffer<float> writeMainBuffer(juce::AudioBuffer<float>& buffer) override;
	void performTimeChange(juce::AudioBuffer<float>& buffer, int time) override;
protected:
	float getCrossfadedSample(float splOld, float splNew);
	float getSampleForTime(double blendedTime, int channel, int i);
	bool isCrossfading = false;
	int accumulatedDelay = 0;
	int crossfadeSpl = 0;
	double crossfadeState = 0.0;
	int	oldTime = 0;
};
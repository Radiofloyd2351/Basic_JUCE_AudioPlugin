#pragma once
#include "AbsDelayProcessor.h"
class BasicSteppedDelayProcessor : public AbsDelayProcessor
{
public:
	BasicSteppedDelayProcessor();
	void init(int channels, int sampleRate, int delaySamples, int delayTimeSamples) override;
	juce::AudioBuffer<float> writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer) override;
	void performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int time) override;
private:
	bool _isCrossfading = false;
	int _crossfadeSpl = 0;
	double _crossfadeState = 0.0;
	int	_oldTime = 0;
};
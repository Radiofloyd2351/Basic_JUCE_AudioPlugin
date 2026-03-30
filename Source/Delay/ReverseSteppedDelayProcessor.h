#pragma once
#include "BasicSteppedDelayProcessor.h"
class ReverseSteppedDelayProcessor :
    public BasicSteppedDelayProcessor
{
public:
    void init(int channels, int sampleRate, int delaySamples, int delayTimeSamples) override;
    juce::AudioBuffer<float> writeMainBuffer(juce::AudioBuffer<float>& buffer) override;
    void performTimeChange(juce::AudioBuffer<float>& buffer, int time) override;
    void incrementWritePointer(int samples) override;
	void mixSignals(juce::AudioBuffer<float>& buffer, float dryWet) const override;
	void writeFeedback(juce::AudioBuffer<float>& buffer, float gain = 1) override;
private:
	juce::AudioBuffer<float> writeForwardBuffer(juce::AudioBuffer<float>& buffer);
	void handleSecondPlayhead();
	int	_oldTime = 0;
	int _forwardReadPtr = 0;
	int _secondPlayhead = 0;
};


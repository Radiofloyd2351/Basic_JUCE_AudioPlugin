#pragma once
#include <JuceHeader.h>

class AbsDelayProcessor
{	
protected:
	int dSpl;
	int dWritePtr = 0;
	int dReadPtr = 0;
	int internalSampleRate;
public:
	int dTime = 0;
	virtual void init(int channels, int sampleRate, int delaySamples, int delayTimeSamples) = 0;
	juce::AudioBuffer<float> dBuffer;
	void writeRingBuffer(int channel, juce::AudioBuffer<float>& buffer, float gain = 1);
	virtual juce::AudioBuffer<float> writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer) = 0;
	void mixSignals(int channel, juce::AudioBuffer<float>& buffer, float dryWet) const;
	void incrementWritePointer(int samples);
};


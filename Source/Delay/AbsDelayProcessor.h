#pragma once
#include <JuceHeader.h>

class AbsDelayProcessor
{	
protected:
	int _internalSampleRate;
	juce::AudioBuffer<float> _tempBuffer;
public:
	int dSpl;
	int dWritePtr = 0;
	int dReadPtr = 0;
	int dTime = 0;
	virtual void init(int channels, int sampleRate, int delaySamples, int delayTimeSamples) = 0;
	static juce::AudioBuffer<float> dBuffer;
	AbsDelayProcessor();
	virtual void writeRingBuffer(int channel, juce::AudioBuffer<float>& buffer, float gain = 1);
	virtual juce::AudioBuffer<float> writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer) = 0;
	void mixSignals(int channel, juce::AudioBuffer<float>& buffer, float dryWet) const;
	virtual void performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int time) = 0;
	virtual void incrementWritePointer(int samples);
};


#pragma once
#include "AbsDelayProcessor.h"
class ReverseSteppedDelayProcessor :
    public AbsDelayProcessor
{
public:
	int dReadPtrButCool[2]{ 0, 0 };
    bool playingReverse = false;
    int blockSamplesPlayed = 0;
    int lastReadPtr = 0;
    int accumulatedDelay = 0;
	int channelsIterated = 0;
    void init(int channels, int sampleRate, int delaySamples, int delayTimeSamples) override;
    juce::AudioBuffer<float> writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer) override;
    void performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int time) override;
    void incrementWritePointer(int samples) override;
	void mixSignals(int channel, juce::AudioBuffer<float>& buffer, float dryWet) const override;
};


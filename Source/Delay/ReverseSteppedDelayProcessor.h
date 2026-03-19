#pragma once
#include "BasicSteppedDelayProcessor.h"
class ReverseSteppedDelayProcessor :
    public BasicSteppedDelayProcessor
{
public:
	int dReadPtrButCool[2]{ 0, 0 };
    bool playingReverse = false;
    int blockSamplesPlayed = 0;
    int lastReadPtr = 0;
    int accumulatedDelay = 0;
	int channelsIterated = 0;
    void init(int channels, int sampleRate, int delaySamples, int delayTimeSamples) override;
    juce::AudioBuffer<float> writeMainBuffer(juce::AudioBuffer<float>& buffer) override;
    void performTimeChange(juce::AudioBuffer<float>& buffer, int time) override;
    void incrementWritePointer(int samples) override;
	void mixSignals(juce::AudioBuffer<float>& buffer, float dryWet) const override;
private:
	int _calculateDistance(int proposedReadPtr);
	bool _isCrossfading = false;
	int _crossfadeSpl = 0;
	double _crossfadeState = 0.0;
	int	_oldTime = 0;
};


#pragma once
#include "AbsDelayProcessor.h"
class ReverseSteppedDelayProcessor :
    public AbsDelayProcessor
{
public:
    bool playingReverse = false;
    int blockSamplesPlayed = 0;
    int delayPtr = 0;
    int accumulatedDelay = 0;
    void init(int channels, int sampleRate, int delaySamples, int delayTimeSamples) override;
    juce::AudioBuffer<float> writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer) override;
    void performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int time) override;
    void incrementWritePointer(int samples) override;
    void reverseBuffer(int channel, int delaySamples);
};


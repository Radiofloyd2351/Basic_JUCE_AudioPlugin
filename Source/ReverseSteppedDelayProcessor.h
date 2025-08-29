#pragma once
#include "AbsDelayProcessor.h"
class ReverseSteppedDelayProcessor :
    public AbsDelayProcessor
{
public:
    int delayPtr = 0;
    int accumulatedDelay = 0;
    void init(int channels, int sampleRate, int delaySamples, int delayTimeSamples) override;
    juce::AudioBuffer<float> writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer) override;
    void performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int time) override;
};


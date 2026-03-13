#include "BasicSteppedDelayProcessor.h"

BasicSteppedDelayProcessor::BasicSteppedDelayProcessor()
{
    dBuffer = juce::AudioBuffer<float>();
}

void BasicSteppedDelayProcessor::init(int channels, int sampleRate, int delaySamples, int delayTime)
{
    _internalSampleRate = sampleRate;
    _dSpl = delaySamples;
    dBuffer.setSize(channels, _dSpl);
    dTime = delayTime;
    dBuffer.clear();
}

juce::AudioBuffer<float> BasicSteppedDelayProcessor::writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer)
{
    _tempBuffer.makeCopyOf(buffer);
    int proposedReadPtr = _dWritePtr - (dTime * _internalSampleRate);
    _dReadPtr = (proposedReadPtr >= 0) ? proposedReadPtr : proposedReadPtr + _dSpl;
    for (int i = 0; i < _tempBuffer.getNumSamples(); i++) {
        float spl = *dBuffer.getReadPointer(channel, (_dReadPtr + i) % _dSpl);
        *_tempBuffer.getWritePointer(channel, i) += spl;
    }
    return _tempBuffer;
}

void BasicSteppedDelayProcessor::performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int time)
{
    if (buffer.getRMSLevel(channel, 0, buffer.getNumSamples()) <= 0.01) {
        dTime = time;
        buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), 0, 1);
    }
    buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), 1, 0);
}


#include "ReverseSteppedDelayProcessor.h"

void ReverseSteppedDelayProcessor::init(int channels, int sampleRate, int delaySamples, int delayTime)
{
    _internalSampleRate = sampleRate;
    _dSpl = delaySamples;
    dBuffer.setSize(channels, _dSpl);
    dTime = delayTime;
    dBuffer.clear();
    delayPtr = 0;
    accumulatedDelay = 0;
}

juce::AudioBuffer<float> ReverseSteppedDelayProcessor::writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer)
{
    _tempBuffer.makeCopyOf(buffer);
    int bufferSpl = _tempBuffer.getNumSamples();
    int proposedReadPtr = _dWritePtr - (dTime * _internalSampleRate);
    _dReadPtr = (proposedReadPtr >= 0) ? proposedReadPtr : proposedReadPtr + _dSpl;
    if(_dWritePtr > delayPtr) delayPtr += dTime * _internalSampleRate;
    DBG(delayPtr);
    if (delayPtr >= _dSpl) delayPtr = dTime * _internalSampleRate;
    for (int i = 0; i < bufferSpl; i++)
    {
        int currentRead = delayPtr - accumulatedDelay - i;
        if (currentRead < 0) 
        {
            currentRead = _dSpl + accumulatedDelay - i;
            accumulatedDelay = 0; 
        }
        float sample = dBuffer.getSample(channel, currentRead);
        buffer.setSample(channel, i, sample);
    }
    return _tempBuffer;
}

void ReverseSteppedDelayProcessor::performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int time)
{
    if (buffer.getRMSLevel(channel, 0, buffer.getNumSamples()) <= 0.01) {
        dTime = time;
        buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), 0, 1);
    }
    buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), 1, 0);
}




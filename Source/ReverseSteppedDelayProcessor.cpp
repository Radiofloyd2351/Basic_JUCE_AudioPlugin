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
    if (accumulatedDelay >= dTime * _internalSampleRate) {
        reverseBuffer(channel, dTime * _internalSampleRate);
        accumulatedDelay = 0;
    }
    int loopCopyNum = (_dReadPtr + bufferSpl) > _dSpl ? (_dReadPtr + bufferSpl) % _dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    _tempBuffer.addFrom(channel, 0, dBuffer.getReadPointer(channel, _dReadPtr), forwardCopyNum);
    _tempBuffer.addFrom(channel, forwardCopyNum, dBuffer.getReadPointer(channel, 0), loopCopyNum);
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

void ReverseSteppedDelayProcessor::incrementWritePointer(int samples)
{
    _dWritePtr = (_dWritePtr + samples) % _dSpl;
    accumulatedDelay += samples;
}

void ReverseSteppedDelayProcessor::reverseBuffer(int channel, int delaySamples)
{
    if (_dReadPtr + delaySamples <= _dSpl) {
        dBuffer.reverse(channel, _dReadPtr, delaySamples);
    }
    else {
        int forwardCopyNum = _dSpl - _dReadPtr;
        int loopCopyNum = delaySamples - forwardCopyNum;
        dBuffer.reverse(channel, _dReadPtr, forwardCopyNum);
        dBuffer.reverse(channel, 0, loopCopyNum);
    }
}




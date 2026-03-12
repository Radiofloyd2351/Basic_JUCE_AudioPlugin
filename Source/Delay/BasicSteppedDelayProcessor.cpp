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
    int bufferSpl = _tempBuffer.getNumSamples();
    int proposedReadPtr = _dWritePtr - (dTime * _internalSampleRate);
    _dReadPtr = (proposedReadPtr >= 0) ? proposedReadPtr : proposedReadPtr + _dSpl;
    int loopCopyNum = (_dReadPtr + bufferSpl) > _dSpl ? (_dReadPtr + bufferSpl) % _dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    _tempBuffer.addFrom(channel, 0, dBuffer.getReadPointer(channel, _dReadPtr), forwardCopyNum);
    _tempBuffer.addFrom(channel, forwardCopyNum, dBuffer.getReadPointer(channel, 0), loopCopyNum);
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


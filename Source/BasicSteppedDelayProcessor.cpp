#include "BasicSteppedDelayProcessor.h"

BasicSteppedDelayProcessor::BasicSteppedDelayProcessor()
{
    dBuffer = juce::AudioBuffer<float>();
}

void BasicSteppedDelayProcessor::init(int channels, int sampleRate, int delaySamples, int delayTime)
{
    internalSampleRate = sampleRate;
    dSpl = delaySamples;
    dBuffer.setSize(channels, dSpl);
    dTime = delayTime;
    dBuffer.clear();
}

juce::AudioBuffer<float> BasicSteppedDelayProcessor::writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer)
{
    juce::AudioBuffer<float> newBuffer = juce::AudioBuffer<float>();
    newBuffer.makeCopyOf(buffer);
    int bufferSpl = newBuffer.getNumSamples();
    int proposedReadPtr = dWritePtr - (dTime * internalSampleRate);
    dReadPtr = (proposedReadPtr >= 0) ? proposedReadPtr : proposedReadPtr + dSpl;
    int loopCopyNum = (dReadPtr + bufferSpl) > dSpl ? (dReadPtr + bufferSpl) % dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    newBuffer.addFrom(channel, 0, dBuffer.getReadPointer(channel, dReadPtr), forwardCopyNum);
    newBuffer.addFrom(channel, forwardCopyNum, dBuffer.getReadPointer(channel, 0), loopCopyNum);
    return newBuffer;
}

void BasicSteppedDelayProcessor::performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int time)
{
    if (buffer.getRMSLevel(channel, 0, buffer.getNumSamples()) <= 0.01) {
        dTime = time;
        buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), 0, 1);
    }
    buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), 1, 0);
}


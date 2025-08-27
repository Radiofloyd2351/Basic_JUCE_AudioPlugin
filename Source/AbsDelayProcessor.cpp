#include "AbsDelayProcessor.h"

AbsDelayProcessor::AbsDelayProcessor()
{
    tempBuffer = juce::AudioBuffer<float>();
}

void AbsDelayProcessor::writeRingBuffer(int channel, juce::AudioBuffer<float>& buffer, float gain)
{
    int bufferSpl = buffer.getNumSamples();
    int loopCopyNum = (dWritePtr + bufferSpl) > dSpl ? (dWritePtr + bufferSpl) % dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    if (gain < 1) {
        dBuffer.addFrom(channel, dWritePtr, buffer.getReadPointer(channel), forwardCopyNum, gain);
        dBuffer.addFrom(channel, 0, buffer.getReadPointer(channel) + forwardCopyNum, loopCopyNum, gain);
    }
    else {
        dBuffer.copyFrom(channel, dWritePtr, buffer.getReadPointer(channel), forwardCopyNum, gain);
        dBuffer.copyFrom(channel, 0, buffer.getReadPointer(channel) + forwardCopyNum, loopCopyNum, gain);
    }
}

void AbsDelayProcessor::mixSignals(int channel, juce::AudioBuffer<float>& buffer, float dryWet) const
{
    int bufferSpl = buffer.getNumSamples();
    int loopCopyNum = (dReadPtr + bufferSpl) > dSpl ? (dReadPtr + bufferSpl) % dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    buffer.applyGain(channel, 0, bufferSpl, 1 - dryWet);
    buffer.addFrom(channel, 0, dBuffer.getReadPointer(channel, dReadPtr), forwardCopyNum, dryWet);
    buffer.addFrom(channel, forwardCopyNum, dBuffer.getReadPointer(channel, 0), loopCopyNum, dryWet);
}

void AbsDelayProcessor::incrementWritePointer(int samples)
{
    dWritePtr = (dWritePtr + samples) % dSpl;
}



#include "AbsDelayProcessor.h"

AbsDelayProcessor::AbsDelayProcessor()
{
    _tempBuffer = juce::AudioBuffer<float>();
    _dSpl = 0;
    _internalSampleRate = 0;
}

void AbsDelayProcessor::writeRingBuffer(int channel, juce::AudioBuffer<float>& buffer, float gain)
{
    int bufferSpl = buffer.getNumSamples();
    int loopCopyNum = (_dWritePtr + bufferSpl) > _dSpl ? (_dWritePtr + bufferSpl) % _dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    if (gain < 1) {
        dBuffer.addFrom(channel, _dWritePtr, buffer.getReadPointer(channel), forwardCopyNum, gain);
        dBuffer.addFrom(channel, 0, buffer.getReadPointer(channel) + forwardCopyNum, loopCopyNum, gain);
    }
    else {
        dBuffer.copyFrom(channel, _dWritePtr, buffer.getReadPointer(channel), forwardCopyNum, gain);
        dBuffer.copyFrom(channel, 0, buffer.getReadPointer(channel) + forwardCopyNum, loopCopyNum, gain);
    }
}

void AbsDelayProcessor::mixSignals(int channel, juce::AudioBuffer<float>& buffer, float dryWet) const
{
    int bufferSpl = buffer.getNumSamples();
    int loopCopyNum = (_dReadPtr + bufferSpl) > _dSpl ? (_dReadPtr + bufferSpl) % _dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    buffer.applyGain(channel, 0, bufferSpl, 1 - dryWet);
    buffer.addFrom(channel, 0, dBuffer.getReadPointer(channel, _dReadPtr), forwardCopyNum, dryWet);
    buffer.addFrom(channel, forwardCopyNum, dBuffer.getReadPointer(channel, 0), loopCopyNum, dryWet);
}

void AbsDelayProcessor::incrementWritePointer(int samples)
{
    _dWritePtr = (_dWritePtr + samples) % _dSpl;
}



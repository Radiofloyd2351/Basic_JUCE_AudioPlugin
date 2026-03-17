#include "AbsDelayProcessor.h"

juce::AudioBuffer<float> AbsDelayProcessor::dBuffer;

AbsDelayProcessor::AbsDelayProcessor()
{
    _tempBuffer = juce::AudioBuffer<float>();
    dSpl = 0;
    _internalSampleRate = 0;
}

void AbsDelayProcessor::writeRingBuffer(int channel, juce::AudioBuffer<float>& buffer, float gain)
{
    int size = dBuffer.getNumSamples();
    for (int i =  0; i < buffer.getNumSamples(); i++) {
        float spl = *buffer.getReadPointer(channel, i);
        if (gain < 1) {
            *dBuffer.getWritePointer(channel, (dWritePtr + i) % size) += spl * gain;
        }
        else {
            *dBuffer.getWritePointer(channel, (dWritePtr + i) % size) = spl;
        }
    }
    dWritePtr = (dWritePtr + dSpl) % dBuffer.getNumSamples();
}

void AbsDelayProcessor::mixSignals(int channel, juce::AudioBuffer<float>& buffer, float dryWet) const
{
    int bufferSpl = buffer.getNumSamples();
    int loopCopyNum = (dReadPtr + bufferSpl) > dSpl ? (dReadPtr + bufferSpl) % dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    buffer.applyGain(channel, 0, bufferSpl, 1 - dryWet);
    for (int i = 0; i < buffer.getNumSamples(); i++) {
        float spl = *dBuffer.getReadPointer(channel, (dReadPtr + i) % dSpl);
        *buffer.getWritePointer(channel, i) += spl * dryWet;
    }
}

void AbsDelayProcessor::incrementWritePointer(int samples)
{
    dWritePtr = (dWritePtr + samples) % dSpl;
}



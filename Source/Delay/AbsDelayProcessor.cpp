#include "AbsDelayProcessor.h"

juce::AudioBuffer<float> AbsDelayProcessor::dBuffer;

AbsDelayProcessor::AbsDelayProcessor()
{
    _tempBuffer = juce::AudioBuffer<float>();
    _dSpl = 0;
    _internalSampleRate = 0;
}

void AbsDelayProcessor::writeRingBuffer(int channel, juce::AudioBuffer<float>& buffer, float gain)
{
    int size = dBuffer.getNumSamples();
    for (int i =  0; i < buffer.getNumSamples(); i++) {
        float spl = *buffer.getReadPointer(channel, i);
        if (gain < 1) {
            *dBuffer.getWritePointer(channel, (_dWritePtr + i) % size) += spl * gain;
        }
        else {
            *dBuffer.getWritePointer(channel, (_dWritePtr + i) % size) = spl;
        }
    }
    _dReadPtr = _dWritePtr;
    _dWritePtr = (_dWritePtr + _dSpl) % dBuffer.getNumSamples();
   
}

void AbsDelayProcessor::mixSignals(int channel, juce::AudioBuffer<float>& buffer, float dryWet) const
{
    int bufferSpl = buffer.getNumSamples();
    int loopCopyNum = (_dReadPtr + bufferSpl) > _dSpl ? (_dReadPtr + bufferSpl) % _dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    buffer.applyGain(channel, 0, bufferSpl, 1 - dryWet);
    for (int i = 0; i < buffer.getNumSamples(); i++) {
        float spl = *dBuffer.getReadPointer(channel, (_dReadPtr + i) % _dSpl);
        *buffer.getWritePointer(channel, i) += spl * dryWet;
    }
}

void AbsDelayProcessor::incrementWritePointer(int samples)
{
    _dWritePtr = (_dWritePtr + samples) % _dSpl;
}



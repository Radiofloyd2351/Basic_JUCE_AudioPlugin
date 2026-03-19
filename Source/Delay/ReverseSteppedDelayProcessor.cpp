#include "ReverseSteppedDelayProcessor.h"

void ReverseSteppedDelayProcessor::init(int channels, int sampleRate, int delaySamples, int delayTime)
{
    _internalSampleRate = sampleRate;
    dSpl = delaySamples;
    dBuffer.setSize(channels, dSpl);
    dTime = delayTime;
    dBuffer.clear();
	dReadPtr = dSpl;
    accumulatedDelay = 0;
}

juce::AudioBuffer<float> ReverseSteppedDelayProcessor::writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer)
{
	_tempBuffer.makeCopyOf(buffer);
	for (int i = 0; i < _tempBuffer.getNumSamples(); i++) {
		int proposedReadPtr = dReadPtr - i - 1 >= 0 ? dReadPtr - i - 1 : dReadPtr - i - 1 + dSpl;
		float spl = *dBuffer.getReadPointer(channel, proposedReadPtr);
		*_tempBuffer.getWritePointer(channel, i) += spl;
	}
	if (channel == 0) {
		accumulatedDelay += buffer.getNumSamples();
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

void ReverseSteppedDelayProcessor::incrementWritePointer(int samples)
{
    dWritePtr = (dWritePtr + samples) % dSpl;
	dReadPtr = (dReadPtr - samples + dSpl) % dSpl;
	if (accumulatedDelay >= dTime * _internalSampleRate) {
		dReadPtr = dWritePtr;
		accumulatedDelay = 0;
	}
}

void ReverseSteppedDelayProcessor::mixSignals(int channel, juce::AudioBuffer<float>& buffer, float dryWet) const {
	int bufferSpl = buffer.getNumSamples();
	int loopCopyNum = (dReadPtr + bufferSpl) > dSpl ? (dReadPtr + bufferSpl) % dSpl : 0;
	int forwardCopyNum = bufferSpl - loopCopyNum;
	buffer.applyGain(channel, 0, bufferSpl, 1 - dryWet);
	for (int i = 0; i < buffer.getNumSamples(); i++) {
		float spl = *dBuffer.getReadPointer(channel, (dReadPtr - i + dSpl) % dSpl);
		*buffer.getWritePointer(channel, i) += spl * dryWet;
	}
}







#include "AbsDelayProcessor.h"

juce::AudioBuffer<float> AbsDelayProcessor::dBuffer;

int AbsDelayProcessor::DBG_ITER = 0;

AbsDelayProcessor::AbsDelayProcessor()
{
    _tempBuffer = juce::AudioBuffer<float>();
    dSpl = 0;
    _internalSampleRate = 0;
}


void AbsDelayProcessor::writeRingBuffer(juce::AudioBuffer<float>& buffer, float gain)
{

		int size = dBuffer.getNumSamples();
		for (int i = 0; i < buffer.getNumSamples(); i++) {
			for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
			float spl = *buffer.getReadPointer(channel, i);
			if (gain < 1)
				*dBuffer.getWritePointer(channel, (dWritePtr + i) % size) += spl * gain;
			else
				*dBuffer.getWritePointer(channel, (dWritePtr + i) % size) = spl;
		}
	}
}

void AbsDelayProcessor::mixSignals(juce::AudioBuffer<float>& buffer, float dryWet) const
{
	for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
		int bufferSpl = buffer.getNumSamples();
		int loopCopyNum = (dReadPtr + bufferSpl) > dSpl ? (dReadPtr + bufferSpl) % dSpl : 0;
		int forwardCopyNum = bufferSpl - loopCopyNum;
		buffer.applyGain(channel, 0, bufferSpl, 1 - dryWet);
		for (int i = 0; i < buffer.getNumSamples(); i++) {
			float spl = *dBuffer.getReadPointer(channel, (dReadPtr - i + dSpl) % dSpl);
			*buffer.getWritePointer(channel, i) += spl * dryWet;
		}
	}
}

void AbsDelayProcessor::incrementWritePointer(int samples)
{
	int lastWritePtr = dWritePtr;
    dWritePtr = (dWritePtr + samples) % dSpl;
}

float AbsDelayProcessor::cubicHermiteInterpolation(float A, float B, float C, float D, float t)
{
	float a = -A / 2.0f + (3.0f * B) / 2.0f - (3.0f * C) / 2.0f + D / 2.0f;
	float b = A - (5.0f * B) / 2.0f + 2.0f * C - D / 2.0f;
	float c = -A / 2.0f + C / 2.0f;
	float d = B;
	return a * t * t * t + b * t * t + c * t + d;
}

float AbsDelayProcessor::linearInterpolation(float v1, float v2, float fraction)
{
	if (fraction >= 1.0)
		return v2;
	return fraction * v2 + (1.0 - fraction) * v1;
}


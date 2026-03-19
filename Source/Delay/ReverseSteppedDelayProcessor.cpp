#include "ReverseSteppedDelayProcessor.h"

const int CROSSFADE_DURATION_MS = 200;

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

int ReverseSteppedDelayProcessor::_calculateDistance(int proposedReadPtr) {
	int forward = (proposedReadPtr - dWritePtr + dSpl) % dSpl;
	int backward = (dWritePtr - proposedReadPtr + dSpl) % dSpl;
	return abs(std::min(forward, backward));
}

juce::AudioBuffer<float> ReverseSteppedDelayProcessor::writeMainBuffer(juce::AudioBuffer<float>& buffer)
{
	for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
		_tempBuffer.makeCopyOf(buffer);
		float crossFadeTimeSpl = CROSSFADE_DURATION_MS * _internalSampleRate / 1000.0f;
		double crossfadeState = static_cast<double>(_crossfadeSpl) / crossFadeTimeSpl;
		_crossfadeState = juce::jlimit(0.0, 1.0, crossfadeState);
		for (int i = 0; i < _tempBuffer.getNumSamples(); i++) {
			int safetySamples = buffer.getNumSamples();
			int proposedReadPtr = dReadPtr - i - 1 >= 0 ? dReadPtr - i - 1 : dReadPtr - i - 1 + dSpl;
			int secondaryReadPtr = (proposedReadPtr - safetySamples + dSpl) % dSpl;
			int distance = _calculateDistance(proposedReadPtr);
			float spl;
			if (distance <= crossFadeTimeSpl) {
				float oldGain = 1.0f - static_cast<float>(_crossfadeState);
				float newGain = static_cast<float>(_crossfadeState);
				float splOld = *dBuffer.getReadPointer(channel, proposedReadPtr);
				float splNew = *dBuffer.getReadPointer(channel, secondaryReadPtr);
				spl = splOld * oldGain + splNew * newGain;
				_crossfadeSpl++;
			}
			else {
				spl = *dBuffer.getReadPointer(channel, proposedReadPtr);
			}
			*_tempBuffer.getWritePointer(channel, i) += spl;
		}
		if (channel == 0) {
			accumulatedDelay += buffer.getNumSamples();
		}

		return _tempBuffer;
	}
}


void ReverseSteppedDelayProcessor::performTimeChange(juce::AudioBuffer<float>& buffer, int time)
{
	for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
		if (buffer.getRMSLevel(channel, 0, buffer.getNumSamples()) <= 0.01) {
			dTime = time;
			buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), 0, 1);
		}
		buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), 1, 0);
	}
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

void ReverseSteppedDelayProcessor::mixSignals(juce::AudioBuffer<float>& buffer, float dryWet) const {
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







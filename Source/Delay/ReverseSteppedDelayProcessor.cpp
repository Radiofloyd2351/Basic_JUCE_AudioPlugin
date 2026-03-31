#include "ReverseSteppedDelayProcessor.h"

const int CROSSFADE_DURATION_MS = 1000;

void ReverseSteppedDelayProcessor::init(int channels, int sampleRate, int delaySamples, int delayTime)
{
    _internalSampleRate = sampleRate;
    dSpl = delaySamples;
    dBuffer.setSize(channels, dSpl);
    dTime = delayTime;
    dBuffer.clear();
	dReadPtr = dSpl;
    accumulatedDelay = 0;
	isReverse = true;
}

juce::AudioBuffer<float> ReverseSteppedDelayProcessor::writeForwardBuffer(juce::AudioBuffer<float>& buffer)
{
	float spl;
	_tempBuffer.makeCopyOf(buffer);
	int proposedReadPtr = dWritePtr - (dTime * _internalSampleRate);
	_forwardReadPtr = (proposedReadPtr >= 0) ? proposedReadPtr : proposedReadPtr + dSpl;
	for (int i = 0; i < _tempBuffer.getNumSamples(); i++) {
		for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
			spl = *dBuffer.getReadPointer(channel, (_forwardReadPtr - i - 1 + dSpl) % dSpl);
			*_tempBuffer.getWritePointer(channel, i) += spl;
		}
	}
	return _tempBuffer;
}

void ReverseSteppedDelayProcessor::handleSecondPlayhead()
{
	int toleranceSamples = (CROSSFADE_DURATION_MS * _internalSampleRate / 1000);

	_secondPlayhead = (dReadPtr - toleranceSamples + dSpl) % dSpl;

	int distance = (dWritePtr - _secondPlayhead + dSpl) % dSpl;

	// CASE 1: overlap risk (write is getting too close to read)
	int start = (dReadPtr - toleranceSamples + dSpl) % dSpl;
	int end = dReadPtr;

	bool isSandwiched;

	if (start <= end)
		isSandwiched = (dWritePtr >= start && dWritePtr <= end);
	else
		isSandwiched = (dWritePtr >= start || dWritePtr <= end);

	if (isSandwiched && !isCrossfading)
	{
		DBG("CROSSING");
		isCrossfading = true;
	}
}

juce::AudioBuffer<float> ReverseSteppedDelayProcessor::writeMainBuffer(juce::AudioBuffer<float>& buffer)
{
	float spl;
	handleSecondPlayhead();
	
	_tempBuffer.makeCopyOf(buffer);
	for (int i = 0; i < _tempBuffer.getNumSamples(); i++) {
		for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
			int proposedReadPtr = dReadPtr - i - 1 >= 0 ? dReadPtr - i - 1 : dReadPtr - i - 1 + dSpl;
			if (isCrossfading) {
				int readOld = (dReadPtr - i - 1 + dSpl) % dSpl;
				int readNew = (_secondPlayhead - i - 1 + dSpl) % dSpl;

				float splOld = *dBuffer.getReadPointer(channel, readOld);
				float splNew = *dBuffer.getReadPointer(channel, readNew);

				spl = getCrossfadedSample(splOld, splNew);
			}
			else {
				spl = *dBuffer.getReadPointer(channel, proposedReadPtr);
			}
			*_tempBuffer.getWritePointer(channel, i) += spl;
		}
		if (isCrossfading) crossfadeSpl++;
		isCrossfading = checkCrossfadeState();
	}
	accumulatedDelay += buffer.getNumSamples();
	return _tempBuffer;
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
	_forwardReadPtr = (_forwardReadPtr + samples) % dSpl;
	/*
	if (accumulatedDelay >= dTime * _internalSampleRate) {
		_secondPlayhead = dWritePtr;
		accumulatedDelay = 0;
	}
	*/
}

void ReverseSteppedDelayProcessor::mixSignals(juce::AudioBuffer<float>& buffer, float dryWet) const {
	for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
		int bufferSpl = buffer.getNumSamples();
		int loopCopyNum = (dReadPtr + bufferSpl) > dSpl ? (dReadPtr + bufferSpl) % dSpl : 0;
		buffer.applyGain(channel, 0, bufferSpl, 1 - dryWet);
		for (int i = 0; i < buffer.getNumSamples(); i++) {
			float spl = *dBuffer.getReadPointer(channel, (dReadPtr + i) % dSpl);
			*buffer.getWritePointer(channel, i) += spl * dryWet;
		}
	}
}

void ReverseSteppedDelayProcessor::writeFeedback(juce::AudioBuffer<float>& buffer, float gain)
{
	auto buff = writeForwardBuffer(buffer);
	writeRingBuffer(buff, gain);
}







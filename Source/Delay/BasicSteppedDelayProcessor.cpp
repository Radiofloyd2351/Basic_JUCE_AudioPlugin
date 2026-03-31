#include "BasicSteppedDelayProcessor.h"

const int CROSSFADE_DURATION_MS = 128;

BasicSteppedDelayProcessor::BasicSteppedDelayProcessor()
{
    dBuffer = juce::AudioBuffer<float>();
}

void BasicSteppedDelayProcessor::init(int channels, int sampleRate, int delaySamples, int delayTime)
{
    _internalSampleRate = sampleRate;
    dSpl = delaySamples;
    dBuffer.setSize(channels, dSpl);
    dTime = delayTime;
    crossfadeSpl = 0;
    crossfadeState = 0;
    isCrossfading = false;
    oldTime = 0;
    dBuffer.clear();
	dReadPtr = dBuffer.getNumSamples() - 1;
}

/**
returns a sample crossfaded between the old sample and the new sample, based on the crossfade duration
constant.
@param splOld: The sample being crossfaded from.
@param splNew: The sample being crossfaded to.
*/
float BasicSteppedDelayProcessor::getCrossfadedSample(float splOld, float splNew)
{
	double crossTimeSpl = CROSSFADE_DURATION_MS / 1000.0 * _internalSampleRate;

	double t = static_cast<double>(crossfadeSpl) / crossTimeSpl;
	t = juce::jlimit(0.0, 1.0, t);

	float oldGain = 1.0f - static_cast<float>(t);
	float newGain = static_cast<float>(t);

	return splOld * oldGain + splNew * newGain;
}

bool BasicSteppedDelayProcessor::checkCrossfadeState()
{
	double crossTimeSpl = CROSSFADE_DURATION_MS / 1000.0 * _internalSampleRate;
	return crossfadeSpl < crossTimeSpl;
}

/**
returns a sample from the delay based on the time values when the time changes.
@param blendedTime: the delay from the write pointer being fetched.
@param channel: the channel currently being fetched from on the delay buffer.
@param i: the current index offset from the read pointer being fetched.
*/
float BasicSteppedDelayProcessor::getSampleForTime(double blendedTime, int channel, int i)
{
	int proposedReadPtr = dWritePtr - (blendedTime * _internalSampleRate);
	int dReadPtr = (proposedReadPtr >= 0) ? proposedReadPtr : proposedReadPtr + dSpl;
	return *dBuffer.getReadPointer(channel, (dReadPtr + i) % dSpl);
}

juce::AudioBuffer<float> BasicSteppedDelayProcessor::writeMainBuffer(juce::AudioBuffer<float>& buffer)
{
		float spl;
		_tempBuffer.makeCopyOf(buffer);		
		int proposedReadPtr = dWritePtr - (dTime * _internalSampleRate);
		dReadPtr = (proposedReadPtr >= 0) ? proposedReadPtr : proposedReadPtr + dSpl;
		for (int i = 0; i < _tempBuffer.getNumSamples(); i++) {
			for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
				if (isCrossfading) {
					float splOld = getSampleForTime(oldTime, channel, i);
					float splNew = getSampleForTime(dTime, channel, i);
					spl = getCrossfadedSample(splOld, splNew);
				}
				else
				{
					spl = *dBuffer.getReadPointer(channel, (dReadPtr + i) % dSpl);
				}
				*_tempBuffer.getWritePointer(channel, i) += spl;
			}
			if (isCrossfading) crossfadeSpl++;
			isCrossfading = checkCrossfadeState();
		}
		return _tempBuffer;
}

void BasicSteppedDelayProcessor::performTimeChange(juce::AudioBuffer<float>& buffer, int time)
{
	for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
		// Crossfade check
		if (buffer.getRMSLevel(channel, 0, buffer.getNumSamples()) <= 0.01) {
			if (!isCrossfading) {
				isCrossfading = true;
				crossfadeSpl = 0;
				crossfadeState = 0;
				oldTime = dTime;
				dTime = time;
			}
		}
	}
}


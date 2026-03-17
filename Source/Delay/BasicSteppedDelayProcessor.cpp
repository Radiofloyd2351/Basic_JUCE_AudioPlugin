#include "BasicSteppedDelayProcessor.h"

const int CROSSFADE_DURATION_MS = 200;

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
    _crossfadeSpl = 0;
    _crossfadeState = 0;
    _isCrossfading = false;
    _oldTime = 0;
    dBuffer.clear();
    dReadPtr = dBuffer.getNumSamples() - 1;

}

juce::AudioBuffer<float> BasicSteppedDelayProcessor::writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer)
{
    _tempBuffer.makeCopyOf(buffer);
    if (_isCrossfading) {
        double crossTimeSpl = CROSSFADE_DURATION_MS / 1000.0 * _internalSampleRate;
        _crossfadeState = static_cast<double>(_crossfadeSpl) / crossTimeSpl;
        _crossfadeState = juce::jlimit(0.0, 1.0, _crossfadeState);

        // Calculate gain factors for old and new delay times
        float oldGain = 1.0f - static_cast<float>(_crossfadeState);
        float newGain = static_cast<float>(_crossfadeState);

        // Process old delay time with oldGain
        double blendedDelayTimeOld = _oldTime;
        int proposedReadPtrOld = dWritePtr - (blendedDelayTimeOld * _internalSampleRate);
        int dReadPtrOld = (proposedReadPtrOld >= 0) ? proposedReadPtrOld : proposedReadPtrOld + dSpl;

        // Process new delay time with newGain
        double blendedDelayTimeNew = dTime;
        int proposedReadPtrNew = dWritePtr - (blendedDelayTimeNew * _internalSampleRate);
        int dReadPtrNew = (proposedReadPtrNew >= 0) ? proposedReadPtrNew : proposedReadPtrNew + dSpl;

        for (int i = 0; i < _tempBuffer.getNumSamples(); i++) {
            float splOld = *dBuffer.getReadPointer(channel, (dReadPtrOld + i) % dSpl);
            float splNew = *dBuffer.getReadPointer(channel, (dReadPtrNew + i) % dSpl);

            *_tempBuffer.getWritePointer(channel, i) += splOld * oldGain + splNew * newGain;
        }

        _crossfadeSpl += _tempBuffer.getNumSamples();
        if (_crossfadeSpl > crossTimeSpl) {
            _isCrossfading = false;
        }
        return _tempBuffer;
    }

    int proposedReadPtr = dWritePtr - (dTime * _internalSampleRate);
    dReadPtr = (proposedReadPtr >= 0) ? proposedReadPtr : proposedReadPtr + dSpl;
    for (int i = 0; i < _tempBuffer.getNumSamples(); i++) {
        float spl = *dBuffer.getReadPointer(channel, (dReadPtr + i) % dSpl);
        *_tempBuffer.getWritePointer(channel, i) += spl;
    }
    return _tempBuffer;
}

void BasicSteppedDelayProcessor::performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int time)
{
    if (buffer.getRMSLevel(channel, 0, buffer.getNumSamples()) <= 0.01) {
        if (!_isCrossfading) {
            _isCrossfading = true;
            _crossfadeSpl = 0;
            _crossfadeState = 0;
            _oldTime = dTime;
            dTime = time;
        }
    }
}


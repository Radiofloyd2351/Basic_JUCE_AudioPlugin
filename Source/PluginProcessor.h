/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class NeedVSToWorkPlsAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    NeedVSToWorkPlsAudioProcessor();
    ~NeedVSToWorkPlsAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float minouVolume = 0;

private:
    juce::AudioProcessorValueTreeState params;
    float dTime;
    int dSpl;
    int dWritePtr = 0;
    int dReadPtr = 0;
    juce::AudioBuffer<float> dBuffer;

    void writeFeedback(int channel, juce::AudioBuffer<float>& buffer, float feedback);
    void writeRingBuffer(int channel, juce::AudioBuffer<float>& buffer);
    juce::AudioBuffer<float> writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer);
    void mixSignals(int channel, juce::AudioBuffer<float>& buffer, float dryWet);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeedVSToWorkPlsAudioProcessor)
};

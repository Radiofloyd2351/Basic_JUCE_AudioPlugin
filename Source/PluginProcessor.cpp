/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

const int MAX_DELAY_TIME_SECONDS = 10;


NeedVSToWorkPlsAudioProcessor::NeedVSToWorkPlsAudioProcessor() : params(*this, nullptr, juce::Identifier("Delay"), {
    std::make_unique<juce::AudioParameterFloat>("DW", "Dry/Wet", 0, 1, 0.25),
    std::make_unique<juce::AudioParameterFloat>("FB", "Feedback", 0, 1, 0.5),
    std::make_unique <juce::AudioParameterInt>("TIME", "Time", 1, MAX_DELAY_TIME_SECONDS, 2)
    }), AudioProcessor(BusesProperties()
#ifndef JucePlugin_PreferredChannelConfigurations

                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    dTime = *params.getRawParameterValue("TIME");
    dBuffer = juce::AudioBuffer<float>();
    dWritePtr = 0;
    dReadPtr = 0;
    dSpl = 0;
}

NeedVSToWorkPlsAudioProcessor::~NeedVSToWorkPlsAudioProcessor()
{
}

//==============================================================================
const juce::String NeedVSToWorkPlsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NeedVSToWorkPlsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NeedVSToWorkPlsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NeedVSToWorkPlsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NeedVSToWorkPlsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NeedVSToWorkPlsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NeedVSToWorkPlsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NeedVSToWorkPlsAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NeedVSToWorkPlsAudioProcessor::getProgramName (int index)
{
    return {};
}

void NeedVSToWorkPlsAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NeedVSToWorkPlsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    dSpl = static_cast<int>((MAX_DELAY_TIME_SECONDS + 1) * sampleRate);
    dBuffer.setSize(getTotalNumInputChannels(), dSpl);
    dBuffer.clear();
}

void NeedVSToWorkPlsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NeedVSToWorkPlsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NeedVSToWorkPlsAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    float feedback = *params.getRawParameterValue("FB");
    float dryWet = *params.getRawParameterValue("DW");
    float nextTime = *params.getRawParameterValue("TIME");
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        writeRingBuffer(channel, buffer);
        if (dTime != nextTime) performTimeChange(channel, buffer, nextTime);
        auto buff = writeMainBuffer(channel, buffer);
        writeRingBuffer(channel, buff, feedback);
        mixSignals(channel, buffer, dryWet);
    }

    dWritePtr = (dWritePtr + buffer.getNumSamples()) % dSpl;
     
}

void NeedVSToWorkPlsAudioProcessor::writeRingBuffer(int channel, juce::AudioBuffer<float>& buffer, float gain )
{
    int bufferSpl = buffer.getNumSamples();
    int loopCopyNum = (dWritePtr + bufferSpl) > dSpl ? (dWritePtr + bufferSpl) % dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    dBuffer.copyFrom(channel, dWritePtr, buffer.getWritePointer(channel), forwardCopyNum, gain);
    dBuffer.copyFrom(channel, 0, buffer.getWritePointer(channel) + forwardCopyNum, loopCopyNum, gain);
}

juce::AudioBuffer<float> NeedVSToWorkPlsAudioProcessor::writeMainBuffer(int channel, juce::AudioBuffer<float>& buffer)
{
    juce::AudioBuffer<float> newBuffer = juce::AudioBuffer<float>();
    newBuffer.makeCopyOf(buffer);
    int bufferSpl = newBuffer.getNumSamples();
    int delaySamples = static_cast<int>(dTime * getSampleRate());
    int proposedReadPtr = dWritePtr - delaySamples;
    dReadPtr = (proposedReadPtr >= 0) ? proposedReadPtr : proposedReadPtr + dSpl;
    int loopCopyNum = (dReadPtr + bufferSpl) > dSpl ? (dReadPtr + bufferSpl) % dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    newBuffer.addFrom(channel, 0, dBuffer.getReadPointer(channel, dReadPtr), forwardCopyNum);
    newBuffer.addFrom(channel, forwardCopyNum, dBuffer.getReadPointer(channel, 0), loopCopyNum);
    return newBuffer;
}

void NeedVSToWorkPlsAudioProcessor::mixSignals(int channel, juce::AudioBuffer<float>& buffer, float dryWet)
{
    int bufferSpl = buffer.getNumSamples();
    int loopCopyNum = (dReadPtr + bufferSpl) > dSpl ? (dReadPtr + bufferSpl) % dSpl : 0;
    int forwardCopyNum = bufferSpl - loopCopyNum;
    buffer.applyGain(channel, 0, bufferSpl, 1 - dryWet);
    buffer.addFrom(channel, 0, dBuffer.getReadPointer(channel, dReadPtr), forwardCopyNum, dryWet);
    buffer.addFrom(channel, forwardCopyNum, dBuffer.getReadPointer(channel, 0), loopCopyNum, dryWet);
}

void NeedVSToWorkPlsAudioProcessor::performTimeChange(int channel, juce::AudioBuffer<float>& buffer, int time) {
    if (buffer.getRMSLevel(channel, 0, buffer.getNumSamples()) <= 0.01) {
        dTime = time;
        buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), 0, 1);
    }
    buffer.applyGainRamp(channel, 0, buffer.getNumSamples(), 1, 0);
}

//==============================================================================
bool NeedVSToWorkPlsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NeedVSToWorkPlsAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void NeedVSToWorkPlsAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void NeedVSToWorkPlsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NeedVSToWorkPlsAudioProcessor();
}

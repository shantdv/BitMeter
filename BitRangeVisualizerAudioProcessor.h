#pragma once

#include <JuceHeader.h>

class BitRangeVisualizerAudioProcessor : public juce::AudioProcessor
{
public:
    BitRangeVisualizerAudioProcessor();
    ~BitRangeVisualizerAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    using AudioProcessor::processBlock; // Fix for processBlock overloading issue

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    std::vector<float> getBitDepthLevels() const { return bitDepthLevels; }
    bool isSampleClipDetected() const { return sampleClipDetected; }
    bool isIntersampleClipDetected() const { return intersampleClipDetected; }

    juce::AudioParameterBool* getShowClipDetectionParam() const { return showClipDetectionParam; }
    juce::AudioParameterBool* getShowSampleRateParam() const { return showSampleRateParam; }

private:
    void calculateBitDepth(const juce::AudioBuffer<float>& buffer);
    void detectClipping(const juce::AudioBuffer<float>& buffer);

    std::vector<float> bitDepthLevels;
    bool sampleClipDetected = false;
    bool intersampleClipDetected = false;

    juce::AudioParameterBool* showClipDetectionParam;
    juce::AudioParameterBool* showSampleRateParam;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BitRangeVisualizerAudioProcessor)
};

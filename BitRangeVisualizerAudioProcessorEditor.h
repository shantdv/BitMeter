#pragma once

#include <JuceHeader.h>
#include "BitRangeVisualizerAudioProcessor.h"

class BitRangeVisualizerAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    BitRangeVisualizerAudioProcessorEditor(BitRangeVisualizerAudioProcessor&);
    ~BitRangeVisualizerAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    BitRangeVisualizerAudioProcessor& processor;
    juce::TooltipWindow tooltipWindow; // Add this line
    juce::Label sampleRateLabel; // Add this line

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BitRangeVisualizerAudioProcessorEditor)
};

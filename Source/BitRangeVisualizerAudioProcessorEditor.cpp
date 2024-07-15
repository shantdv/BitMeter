#include "BitRangeVisualizerAudioProcessorEditor.h"

BitRangeVisualizerAudioProcessorEditor::BitRangeVisualizerAudioProcessorEditor(BitRangeVisualizerAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), tooltipWindow(this) // Initialize tooltipWindow
{
    setSize(200, 450); // Adjusted size to make room for the messages
    startTimerHz(30);  // Update the UI at 30 Hz

    // Initialize the sample rate label
    sampleRateLabel.setText("Sample Rate: " + juce::String(processor.getSampleRate()), juce::dontSendNotification);
    addAndMakeVisible(sampleRateLabel);
}

BitRangeVisualizerAudioProcessorEditor::~BitRangeVisualizerAudioProcessorEditor()
{
    stopTimer();
}

void BitRangeVisualizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto& bitDepthLevels = processor.getBitDepthLevels();
    if (bitDepthLevels.empty()) {
        DBG("Bit depth levels data is empty.");
        return;
    }

    const int numChannels = processor.getTotalNumInputChannels(); // Dynamically determine channels
    const int bitDepthDisplayLimit = 32; // Fixed for 32 bits

    if (bitDepthLevels.size() < static_cast<size_t>(numChannels * bitDepthDisplayLimit)) {
        DBG("Bit depth levels array size is less than expected.");
        return;
    }

    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);

    const float lineHeight = (getHeight() - 60) / float(bitDepthDisplayLimit); // Adjust for compactness
    const float center = getWidth() / 2.0f;
    const float maxWidth = center - 10.0f; // Margin for the lines

    for (int bit = 0; bit < bitDepthDisplayLimit; ++bit)
    {
        float usageLeft = bitDepthLevels[bit];
        float usageRight = bitDepthLevels[bitDepthDisplayLimit + bit];
        float lineWidthLeft = usageLeft * maxWidth;
        float lineWidthRight = usageRight * maxWidth;

        // Calculate yPos without reversing the bit order
        float yPos = 10.0f + bit * lineHeight;

        // Draw horizontal lines
        g.drawLine(center - lineWidthLeft, yPos, center, yPos, 1.0f); // Line width set to 1.0 for better visibility
        g.drawLine(center, yPos, center + lineWidthRight, yPos, 1.0f);

        // Draw bit labels in the center with corrected order
        g.drawText(juce::String(bitDepthDisplayLimit - bit), center - 10, yPos - (lineHeight / 2), 20, lineHeight, juce::Justification::centred);
    }

    if (processor.isSampleClipDetected() && *(processor.getShowClipDetectionParam()))
    {
        g.setColour(juce::Colours::red);
        g.drawText("Sample Clipping Detected", 10, getHeight() - 60, getWidth() - 20, 20, juce::Justification::centred);
    }

    if (processor.isIntersampleClipDetected() && *(processor.getShowClipDetectionParam()))
    {
        g.setColour(juce::Colours::orange);
        g.drawText("Intersample Clipping Detected", 10, getHeight() - 40, getWidth() - 20, 20, juce::Justification::centred);
    }
}

void BitRangeVisualizerAudioProcessorEditor::resized()
{
    // Set the bounds for the sample rate label
    sampleRateLabel.setBounds(10, getHeight() - 20, getWidth() - 20, 20);
}

void BitRangeVisualizerAudioProcessorEditor::timerCallback()
{
    repaint();
    if (*(processor.getShowSampleRateParam()))
    {
        sampleRateLabel.setText("Sample Rate: " + juce::String(processor.getSampleRate()), juce::dontSendNotification); // Update label with current sample rate
        sampleRateLabel.setVisible(true);
    }
    else
    {
        sampleRateLabel.setVisible(false);
    }
}

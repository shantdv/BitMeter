#include "BitRangeVisualizerAudioProcessor.h"
#include "BitRangeVisualizerAudioProcessorEditor.h"
#include <cmath> // for log2 and sqrt

BitRangeVisualizerAudioProcessor::BitRangeVisualizerAudioProcessor()
{
    addParameter(showClipDetectionParam = new juce::AudioParameterBool("showClipDetection", "Show Clip Detection", true));
    addParameter(showSampleRateParam = new juce::AudioParameterBool("showSampleRate", "Show Sample Rate", true));
}

BitRangeVisualizerAudioProcessor::~BitRangeVisualizerAudioProcessor()
{
}

void BitRangeVisualizerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
}

void BitRangeVisualizerAudioProcessor::releaseResources()
{
}

void BitRangeVisualizerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    calculateBitDepth(buffer);
    detectClipping(buffer);
}

void BitRangeVisualizerAudioProcessor::detectClipping(const juce::AudioBuffer<float>& buffer)
{
    sampleClipDetected = false;
    intersampleClipDetected = false;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        const float* samples = buffer.getReadPointer(channel);

        for (int i = 0; i < numSamples; ++i)
        {
            if (samples[i] >= 1.0f || samples[i] <= -1.0f)
            {
                sampleClipDetected = true;
            }

            if (i > 0)
            {
                float prevSample = samples[i - 1];
                float currentSample = samples[i];
                if (std::abs(prevSample - currentSample) > 1.0f)
                {
                    intersampleClipDetected = true;
                }
            }

            if (sampleClipDetected && intersampleClipDetected)
            {
                return; // Early exit if both types of clipping are detected
            }
        }
    }
}

//Calc BitDepth
void BitRangeVisualizerAudioProcessor::calculateBitDepth(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    bitDepthLevels.resize(numChannels * 32, 0.0f); // Resize for 32 bits

    for (int channel = 0; channel < numChannels; ++channel)
    {
        const float* samples = buffer.getReadPointer(channel);

        for (int i = 0; i < numSamples; ++i)
        {
            float sample = samples[i];

            // Scale sample from -1.0 to 1.0 range to 0 to 1.0 range
            sample = (sample + 1.0f) * 0.5f;

            // Convert to integer representation
            uint32_t intSample = static_cast<uint32_t>(sample * 0xFFFFFFFF);

            // Loop through each bit in the sample
            for (int bit = 0; bit < 32; ++bit)
            {
                if (intSample & (1 << bit))
                {
                    // Increment bit usage counter if the bit is set
                    bitDepthLevels[channel * 32 + bit]++;
                }
            }
        }
    }

    // Normalize the bit depth levels
    for (int channel = 0; channel < numChannels; ++channel)
    {
        for (int bit = 0; bit < 32; ++bit)
        {
            bitDepthLevels[channel * 32 + bit] /= numSamples;
        }
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BitRangeVisualizerAudioProcessor();
}

void BitRangeVisualizerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Store plugin state
    auto state = std::make_unique<juce::XmlElement>("State");
    state->setAttribute("showClipDetection", (int)*showClipDetectionParam);
    state->setAttribute("showSampleRate", (int)*showSampleRateParam);
    copyXmlToBinary(*state, destData);
}

void BitRangeVisualizerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state
    std::unique_ptr<juce::XmlElement> state(getXmlFromBinary(data, sizeInBytes));
    if (state != nullptr)
    {
        if (state->hasTagName("State"))
        {
            *showClipDetectionParam = (bool)state->getIntAttribute("showClipDetection", 1);
            *showSampleRateParam = (bool)state->getIntAttribute("showSampleRate", 1);
        }
    }
}

juce::AudioProcessorEditor* BitRangeVisualizerAudioProcessor::createEditor()
{
    return new BitRangeVisualizerAudioProcessorEditor(*this);
}

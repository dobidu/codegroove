#pragma once

#include <JuceHeader.h>

#include <barrier>
#include <memory>

namespace py = pybind11;

//==============================================================================
/**
*/
class CodeGrooveAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
                             , public juce::Thread
{
public:
    //==============================================================================
    CodeGrooveAudioProcessor();
    ~CodeGrooveAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

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

    //==============================================================================
    void setPythonCode(juce::String code);
    void run() override;
    std::unique_ptr<PyConfig> setupPythonConfig(std::function<juce::MemoryBlock (const char*)> standardLibraryCallback);
private:
    //==============================================================================
    juce::String pythonCode;
    juce::AudioBuffer<float> audioBuffer;
    std::barrier<> audioReady{2};
    std::barrier<> pythonReady{2};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CodeGrooveAudioProcessor)
};
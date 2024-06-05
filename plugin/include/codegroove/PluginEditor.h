/* Description: This file contains the declaration of the 
CodeGrooveAudioProcessorEditor class, which is responsible for the GUI of the 
plugin. It contains the declarations of the ConfigWindow and ContentComponent
classes, which are used for setting the API key and audio playback, 
respectively.
*/

#pragma once

#include <JuceHeader.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "PluginProcessor.h"

class CodeGrooveAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CodeGrooveAudioProcessorEditor (CodeGrooveAudioProcessor&);
    ~CodeGrooveAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CodeGrooveAudioProcessor& audioProcessor;

    void loadFile();
    void onClick_Play();
    void onClick_Stop();
    void onClick_Save();
    void drawAndConfigComponents();

    juce::String file_path;

    juce::Label file_lbl;
    juce::Label lang_selector_lbl;
    
    juce::TextButton loadfile_btn;
    juce::TextButton play_btn;
    juce::TextButton stop_btn;
    juce::TextButton save_btn;
    juce::TextButton config_btn;

    juce::TextEditor file_input;

    juce::TextEditor code_text;
    juce::TextEditor debug_text;

    juce::ComboBox lang_selector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CodeGrooveAudioProcessorEditor)
};

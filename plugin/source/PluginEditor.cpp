/*
Description: This file contains the implementation of the PluginEditor class,
which is responsible for the graphical interface of the plugin. The PluginEditor class 
has a constructor that receives a reference to the CodeGrooveAudioProcessor class, which
is the class that implements the audio processing logic of the plugin. 

*/

#include <cpr/cpr.h>

#include "codegroove/PluginProcessor.h"
#include "codegroove/PluginEditor.h"

//TODO: rename classes (orchestrator pad instead of CodeGroove)
//TODO: namespace for classes

//==============================================================================
CodeGrooveAudioProcessorEditor::CodeGrooveAudioProcessorEditor (CodeGrooveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)

{

    juce::Component::setSize (480, 540);
    drawAndConfigComponents();


}

CodeGrooveAudioProcessorEditor::~CodeGrooveAudioProcessorEditor()
{

}


using ReplyFunc = std::function<void(const juce::String&)>;


void CodeGrooveAudioProcessorEditor::loadFile()
{
    juce::FileChooser chooser("Load file...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory),
        "*",
        true,
        false);

    if (chooser.browseForFileToOpen())
    { 
        juce::File f;
        f = chooser.getResult();
        file_path = f.getFullPathName();

        int last_bar = file_path.lastIndexOfChar('\\');
        file_lbl.setText(file_path.substring(last_bar+1), juce::dontSendNotification);

    }
}



void CodeGrooveAudioProcessorEditor::drawAndConfigComponents() {
    // Load  File
    addAndMakeVisible(file_lbl);
    addAndMakeVisible(loadfile_btn);
    loadfile_btn.setButtonText("Load Code File");
    loadfile_btn.onClick = [this] { loadFile(); };


    // Lang Selector
    addAndMakeVisible(lang_selector_lbl);
    lang_selector_lbl.setText("Language: ", juce::dontSendNotification);
    lang_selector_lbl.attachToComponent(&lang_selector, true);
    addAndMakeVisible(lang_selector);
    if (lang_selector.getNumItems() == 0) {
        lang_selector.setEnabled(false);
    } else {
        lang_selector.setEnabled(true);
    }

    
    addAndMakeVisible(play_btn);
    play_btn.setButtonText("Play");
    play_btn.setColour(juce::TextButton::buttonColourId, juce::Colours::green);

    play_btn.onClick = [this] { onClick_Play(); };

    addAndMakeVisible(stop_btn);
    stop_btn.setButtonText("Stop");
    stop_btn.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stop_btn.onClick = [this] { onClick_Stop(); };

    // Save Button
    addAndMakeVisible(save_btn);
    save_btn.setButtonText("Save");
    save_btn.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
    save_btn.onClick = [this] { onClick_Save(); };

    addAndMakeVisible(code_text);
    code_text.setMultiLine(true);
    code_text.setReadOnly(false);

    addAndMakeVisible(debug_text);
    debug_text.setMultiLine(true);
    debug_text.setReadOnly(true);

    loadfile_btn.setBounds(10, 20, 100, 30);
    file_lbl.setBounds(120, 20, 140, 30);
    file_lbl.setJustificationType(juce::Justification::topLeft);
    file_lbl.setFont(juce::Font(18.0f, juce::Font::bold));

    lang_selector_lbl.setBounds(10, loadfile_btn.getY() + 50, 100, 20);
    lang_selector_lbl.setJustificationType(juce::Justification::topLeft);
    lang_selector.setBounds(120, loadfile_btn.getY() + 50, (int)(getWidth() * 0.65), 30);

    const int startX = (getWidth() - ((3*60)+20)) / 2;  //3 buttons + 20px spacing

    play_btn.setBounds(startX, lang_selector.getY() + 50, 60, 40);
    stop_btn.setBounds(play_btn.getRight() + 10, lang_selector.getY() + 50, 60, 40);
    save_btn.setBounds(stop_btn.getRight() + 10, lang_selector.getY() + 50, 60, 40);

    code_text.setBounds(10, play_btn.getY() + 60, getWidth() - 20, 160);
    debug_text.setBounds(10, code_text.getY() + 180, getWidth() - 20, 160);

}


void CodeGrooveAudioProcessorEditor::onClick_Play()
{

}

void CodeGrooveAudioProcessorEditor::onClick_Stop()
{

}


void CodeGrooveAudioProcessorEditor::onClick_Save()
{
/*
    juce::FileChooser chooser("Save file...",
        juce::File::getSpecialLocation(juce::File::userHomeDirectory).getChildFile(output_selector.getText().toStdString()),
        "*.py",
        true,
        false);

    if (chooser.browseForFileToSave(true))
    {
        juce::File f;
        f = chooser.getResult();
        juce::String output_file_path = 
        juce::File::getSpecialLocation(juce::File::tempDirectory).getFullPathName() +
        juce::File::getSeparatorString() +
        String("orch-pad") + 
        juce::File::getSeparatorString() + 

    }
*/
}

//==============================================================================
void CodeGrooveAudioProcessorEditor::paint (juce::Graphics& g)
{

}

void CodeGrooveAudioProcessorEditor::resized()
{

}

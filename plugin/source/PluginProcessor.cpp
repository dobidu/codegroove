#include "codegroove/PluginProcessor.h"
#include "codegroove/PluginEditor.h"

//==============================================================================

JUCE_PYTHON_DEFINE_EMBEDDED_MODULES 

//==============================================================================
CodeGrooveAudioProcessor::CodeGrooveAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
      AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , 
#endif
    Thread("CodeGroovePython Thread")
{
    setPythonCode(juce::String());
    
    startThread();

    std::cout << "Ready\n";
}

CodeGrooveAudioProcessor::~CodeGrooveAudioProcessor()
{
    signalThreadShouldExit();

    audioReady.arrive_and_wait();

    waitForThreadToExit(10000);
}

//==============================================================================
const juce::String CodeGrooveAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CodeGrooveAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CodeGrooveAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CodeGrooveAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CodeGrooveAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CodeGrooveAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CodeGrooveAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CodeGrooveAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String CodeGrooveAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void CodeGrooveAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}


//==============================================================================
void CodeGrooveAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate);

    audioBuffer.setSize(2, samplesPerBlock);
}

void CodeGrooveAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CodeGrooveAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void CodeGrooveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i){
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // Do the buffer swap with the processed buffer from python
    if(isThreadRunning() && !threadShouldExit()){
        for (int i = 0; i < buffer.getNumChannels(); i++){
            audioBuffer.copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
        }

        audioReady.arrive_and_wait();
        pythonReady.arrive_and_wait();

        std::swap(buffer, audioBuffer);
    }
}

//==============================================================================
bool CodeGrooveAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CodeGrooveAudioProcessor::createEditor()
{
    return new CodeGrooveAudioProcessorEditor (*this);
}

//==============================================================================
void CodeGrooveAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ignoreUnused (destData);
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CodeGrooveAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CodeGrooveAudioProcessor();
}

//==============================================================================
void CodeGrooveAudioProcessor::setPythonCode(juce::String code){
    pythonCode = code;
}

void CodeGrooveAudioProcessor::run(){
    auto engine = new popsicle::ScriptEngine(setupPythonConfig(
        [](const char* resourceName) -> juce::MemoryBlock {
            int dataSize = 0;
            auto data = BinaryData::getNamedResource(resourceName, dataSize);
            return { data, static_cast<size_t>(dataSize) };
        }
    ));

    py::dict locals;
    py::gil_scoped_acquire guard{};

    // Import python libraries
    try {
        locals["juce_audio_buffer"] = py::module_::import("popsicle").attr("AudioSampleBuffer");
        locals["np"] = py::module_::import("numpy");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    
    // Process buffer
    while(!threadShouldExit()){
        audioReady.arrive_and_wait();

        locals["buffer"] = audioBuffer;

        auto result = engine->runScript(pythonCode.toRawUTF8(), locals);

        if(result.failed()){
            std::cout << result.getErrorMessage();
        }

        pythonReady.arrive_and_wait();
    }
}

//==============================================================================
std::unique_ptr<PyConfig> CodeGrooveAudioProcessor::setupPythonConfig(std::function<juce::MemoryBlock (const char*)> standardLibraryCallback)
{
    juce::String pythonFolderName, pythonArchiveName;
    juce::String projectName = getName();
    pythonFolderName << "python" << PY_MAJOR_VERSION << "." << PY_MINOR_VERSION;
    pythonArchiveName << "python" << PY_MAJOR_VERSION << PY_MINOR_VERSION << "_zip";

    auto tempPath = juce::File::getSpecialLocation(juce::File::tempDirectory);
    //tempPath.deleteRecursively();

    if (!tempPath.isDirectory()) tempPath.createDirectory();

    auto libPath = tempPath.getChildFile("lib");
    if (!libPath.isDirectory()) libPath.createDirectory();

    auto pythonPath = libPath.getChildFile(pythonFolderName);
    if (!pythonPath.isDirectory()) pythonPath.createDirectory();

    if (!pythonPath.getChildFile("lib-dynload").isDirectory())
    {
        juce::MemoryBlock mb = standardLibraryCallback(pythonArchiveName.toRawUTF8());

        auto mis = juce::MemoryInputStream (mb.getData(), mb.getSize(), false);

        auto zip = juce::ZipFile(mis);
        zip.uncompressTo(pythonPath);
    }

    auto config = std::make_unique<PyConfig>();

    PyConfig_InitPythonConfig (config.get());
    config->parse_argv = 0;
    config->isolated = 1;
    config->install_signal_handlers = 0;
    config->program_name = Py_DecodeLocale(projectName.toRawUTF8(), nullptr);
    config->home = Py_DecodeLocale(tempPath.getFullPathName().toRawUTF8(), nullptr);
    config->pythonpath_env = Py_DecodeLocale(pythonPath.getChildFile("site-packages").getFullPathName().toRawUTF8(), nullptr);

    return config;
}
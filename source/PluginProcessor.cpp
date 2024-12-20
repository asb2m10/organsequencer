#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <stdarg.h>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ) {

    // avoid creating the log file if it is in standalone mode
    if ( !JUCEApplication::isStandaloneApp() ) {
        Logger *tmp = Logger::getCurrentLogger();
        if ( tmp == NULL ) {
            Logger::setCurrentLogger(FileLogger::createDateStampedLogger("OrgranSeq", "DebugSession-", "log", "OrganSeq Created"));
        }
    }
    TRACE("Hi");
    
    ValueTree root(IDs::ROOT);
    root.setProperty(IDs::bpm, 120, nullptr);
    root.setProperty(IDs::internalSeq, false, nullptr);
    internalSeq.referTo(root, IDs::internalSeq, nullptr);
    internalSeq = false;
    ValueTree patterns(IDs::PATTERNS);
    for(int i=0;i<NUM_PATTERN;i++) {
        patterns.addChild(pattern[i].value, -1, nullptr);
    }
    root.addChild(patterns,-1, nullptr);

    ValueTree vtTriggers(IDs::TRIGGERS);
    for(int i=0;i<NUM_SEQ;i++) {
        vtTriggers.addChild(triggers[i].vtTrigger, i, nullptr);
        triggers[i].vtTrigger.setProperty(IDs::triggerMidi, 47-i, nullptr);
    }
    root.addChild(vtTriggers,-1, nullptr);

    rootVt = root;
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    sequencer.setSampleRate(sampleRate);
    for(int i=0;i<NUM_SEQ;i++) {
        triggers[i].setTriggerLength(0.5 * sampleRate);
    }
    samplePpq = ( 60.0 / ( bpm ) ) * sampleRate;
    ppqWindow = ((double)samplesPerBlock) / samplePpq;
    TRACE("ppqWindow %f %f", samplePpq, ppqWindow);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if ( internalSeq ) {
        sequencer.setPos(110, internalJiffies, buffer.getNumSamples());
        internalJiffies += ppqWindow;
    } else {
        auto *playhead = getPlayHead();
        if (playhead != NULL) {
            juce::AudioPlayHead::CurrentPositionInfo posInfo;
            playhead->getCurrentPosition(posInfo);
            if ( posInfo.isPlaying || posInfo.isRecording ) {
                sequencer.setPos(posInfo.bpm, posInfo.ppqPosition, buffer.getNumSamples());
                for (const auto meta : midiMessages) {
                    const auto msg = meta.getMessage();
                    if ( msg.isAllNotesOff() ) {
                        for(int i=0;i<NUM_PATTERN;i++) {
                            pattern[i].active = false;
                        }
                    }
                    int target = msg.getNoteNumber() - 12;
                    if ( target >= 0 || target < 8 ) {
                        pattern[target].active = msg.isNoteOn();
                    }
                }
            } else {
                sequencer.invalidatePos();
                for(int i=0;i<NUM_PATTERN;i++) {
                    pattern[i].active = false;
                }
            }
        } else {
            sequencer.invalidatePos();
            for(int i=0;i<NUM_PATTERN;i++) {
                pattern[i].active = false;
            }
        }
    }
    midiMessages.clear();

    for(int i=0;i<NUM_PATTERN;i++)
        pattern[i].process(triggers, sequencer);

    for(int i=0;i<NUM_SEQ;i++) {
        triggers[i].advance(midiMessages, buffer.getNumSamples());
    }

    for (const auto meta : midiMessages) {
        const auto msg = meta.getMessage();
        TRACE("%f %s", msg.getTimeStamp(), msg.getDescription().toRawUTF8());
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

void dexed_trace(const char *source, const char *fmt, ...) {
    char output[4096];
    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(output, 4095, fmt, argptr);
    va_end(argptr);

    String dest;
    dest << source << " " << output;
    Logger::writeToLog(dest);
}
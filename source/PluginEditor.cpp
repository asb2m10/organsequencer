#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ext/value_tree_debugger.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p) {
    addAndMakeVisible(patternEditor);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (700, 600);

    patternEditor.setActivePattern(processorRef.rootVt.getChildWithName(IDs::PATTERNS).getChild(0));
    patternEditor.setTriggers(processorRef.rootVt.getChildWithName(IDs::TRIGGERS));
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g) {
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized() {
    int width = getWidth();
    int height = getHeight();

    patternEditor.setBounds(5, 5, width - 5, height - 170);
}

void AudioPluginAudioProcessorEditor::mouseDown(const MouseEvent &event) {
    auto vd = new ValueTreeDebugger();
    vd->setSource(processorRef.rootVt);
}
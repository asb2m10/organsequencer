#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ext/value_tree_debugger.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p) {
    addAndMakeVisible(patternEditor);
    addAndMakeVisible(bpm);
    bpm.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
    bpm.setSliderSnapsToMousePosition(false);
    bpm.setColour(Slider::trackColourId, Colours::transparentBlack);
    bpm.setRange(1,63, 1);

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

    bpm.setBounds(5, 5, 40, 20);
    patternEditor.setBounds(5, 50, width - 5, height - 230);
}

void AudioPluginAudioProcessorEditor::mouseDown(const MouseEvent &event) {
    auto vd = new ValueTreeDebugger();
    vd->setSource(processorRef.rootVt);
}
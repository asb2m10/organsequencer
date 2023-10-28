#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ext/value_tree_debugger.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    addAndMakeVisible(patternEditor);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 500);

    patternEditor.setValue(processorRef.rootVt.getChildWithName(IDs::PATTERNS).getChild(0));
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    int width = getWidth();
    int height = getHeight();

    patternEditor.setBounds(5, 5, width - 10, height - 10);
}

void AudioPluginAudioProcessorEditor::mouseDown(const MouseEvent &event) {
        //printf("ref %s\n", processorRef.rootVt.toXmlString().toRawUTF8());

        auto vd = new ValueTreeDebugger();
        vd->setSource(processorRef.rootVt);
}
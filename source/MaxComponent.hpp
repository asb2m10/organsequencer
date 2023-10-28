#pragma once

#include <stdlib.h>
#include "PluginProcessor.h"

const StringArray PPQ_VALUES    = { "1/1", "1/2", "1/4", "1/8", "1/16" };
const float       PPQ_CORRESP[] = {   4.0,   2.0,   1.0,   0.5,   0.25 };

class StepEditor : public Component {
    int values[64];

    ValueTree vt;
    int size = 1;
    int ratio = 1;

    int lastActive;
    int lastAction;
public:
    void setSize(int s) {
        size = s;
    }

    void setValues(ValueTree &vt) {
        this->vt = vt;
        String target = vt.getProperty(IDs::arrayValue);

        for(int i=0;i<target.length();i++) {
            if ( target[i] == '0' ) {
                values[i] = 0;
            } else {
                values[i] = 1;
            }
        }

    }

    void paint(juce::Graphics &g) {
        int ratio = getWidth() / size;
        g.fillAll(Colours::darkgrey);

        for(int i=0;i<size;i++) {
            if ( values[i] != 1 )
                g.setColour(juce::Colours::orange);
            else
                g.setColour(juce::Colours::black);
            g.fillRect(i*ratio, 0, ratio, getHeight());
        }
    }

    void mouseDown(const MouseEvent &event) override {
        if ( event.mods.isRightButtonDown() )
            return;

        int ratio = getWidth() / size;
        int target = event.getPosition().getX() / ratio;
        jassert(target < size);
        lastActive = target;
        lastAction = ! values[lastActive];
        values[lastActive] = lastAction;
        publish();
        repaint();
    }

    void mouseDrag(const MouseEvent &event) override {
        if ( event.mods.isRightButtonDown() )
            return;

        int ratio = getWidth() / size;
        int target = event.getPosition().getX() / ratio;

        if ( target == lastActive )
            return;
        if ( target > size )
            return;
        if ( target < 0 )
            return;
        lastActive = target;
        values[lastActive] = lastAction;
        publish();
        repaint();
    }

    void publish() {
        char fstr[65];

        for(int i=0;i<size;i++) {
            if ( values[i] == 0 )
                fstr[i] = '0';
            else
                fstr[i] = '1';
        }
        fstr[size] = 0;

        vt.setProperty(IDs::arrayValue, String(fstr), nullptr);
    }
};

class RowEditor : public Component {
    ComboBox primPpq;
    ValueTree values;;
    StepEditor stepEditor;
    Slider size;
    Slider drift;

public:
    RowEditor() {
        addAndMakeVisible(stepEditor);
        addAndMakeVisible(size);
        addAndMakeVisible(primPpq);
        addAndMakeVisible(drift);

        primPpq.addItemList(PPQ_VALUES, 1);

        size.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
        size.setSliderSnapsToMousePosition(false);
        size.setColour(Slider::trackColourId, Colours::transparentBlack);
        size.setRange(1,63, 1);
        size.onValueChange = [this] {
            int newSize = (int)size.getValue();
            values.setProperty(IDs::arraySize, newSize, nullptr);
            stepEditor.setSize(newSize);
            stepEditor.repaint();
        };

        drift.setSliderStyle(Slider::SliderStyle::Rotary);
        drift.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        drift.setRange(-4, 4, 0.25);
    }

    void paint(juce::Graphics &g) {
        g.fillAll(Colours::darkgrey);
    }

    void resized() {
        primPpq.setBounds(10, 5, 70, 30);
        drift.setBounds(90, 5, 50, 30);
        stepEditor.setBounds(160, 5, getWidth() - 160 - 50, 30);
        size.setBounds(getWidth() - 40, 5, 30, 30);
    }

    void setValue(ValueTree vt) {
        int targetSize = vt.getProperty(IDs::arraySize);
        size.setValue(targetSize, NotificationType::dontSendNotification);
        stepEditor.setValues(vt);
        stepEditor.setSize(targetSize);
        values = vt;
        repaint();
    }
};

class PatternEditor : public Component {
    RowEditor rowEditors[8];

public:
    PatternEditor() {
        for(int i=0;i<8;i++) {
            addAndMakeVisible(rowEditors[i]);
        }
    }

    void resized() {
        int ratio = (getHeight()-10) / 8;
        for(int i=0;i<8;i++) {
            rowEditors[i].setBounds(5, (i * ratio) + 5, getWidth()-10, ratio - 10);
        }
    }

    void paint(juce::Graphics &g) {
        //g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
        g.fillAll(Colours::lightgrey);
    }

    void setValue(ValueTree vt) {
        for(int i=0;i<8;i++) {
            rowEditors[i].setValue(vt.getChild(i));
        }
    }
};
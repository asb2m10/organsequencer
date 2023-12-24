#pragma once

#include <stdlib.h>
#include "PluginProcessor.h"

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
        float ratio = ((float)getWidth()) / size;
        g.fillAll(Colours::darkgrey);

        for(int i=0;i<size;i++) {
            if ( values[i] != 0 )
                g.setColour(juce::Colours::orange);
            else
                g.setColour(juce::Colours::black);
            int x = i*ratio;

            g.fillRect( ratio*i , 0., ratio, ((float)getHeight()) );
        }
    }

    void mouseDown(const MouseEvent &event) override {
        if ( event.mods.isRightButtonDown() )
            return;

        float ratio = ((float)getWidth()) / size;
        float target = (int) (((float)event.getPosition().getX()) / ratio);
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

        float ratio = ((float)getWidth()) / size;
        float target = (int) (((float)event.getPosition().getX()) / ratio);

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
    Slider size;
    Slider trigger;
    Slider drift;
    TextButton muted;
    StepEditor stepEditor;

    ValueTree values;
    ValueTree vtTrigger;

public:
    RowEditor() {
        addAndMakeVisible(stepEditor);
        addAndMakeVisible(size);
        addAndMakeVisible(primPpq);
        addAndMakeVisible(drift);
        addAndMakeVisible(trigger);
        addAndMakeVisible(muted);

        primPpq.addItemList(PPQ_VALUES, 1);
        primPpq.onChange = [this] {
            values.setProperty(IDs::arrayPpq, primPpq.getSelectedItemIndex(), nullptr);
        };

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

        trigger.setSliderStyle(Slider::SliderStyle::LinearBarVertical);
        trigger.setSliderSnapsToMousePosition(false);
        trigger.setColour(Slider::trackColourId, Colours::transparentBlack);
        trigger.setRange(1, 127, 1);
        trigger.onValueChange = [this] {
            int newTrigger = (int)trigger.getValue();
            vtTrigger.setProperty(IDs::triggerMidi, newTrigger, nullptr);
        };

        muted.setClickingTogglesState(true);
        muted.setButtonText("M");

        drift.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
        drift.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        drift.setRange(-4, 4, 0.25);
    }

    void paint(juce::Graphics &g) {
        g.fillAll(Colours::darkgrey);
    }

    void resized() {
        primPpq.setBounds(5, 5, 75, 30);
        drift.setBounds(78, 5, 90, 30);
        stepEditor.setBounds(162, 5, getWidth() - 162 - 70 - 50, 30);
        muted.setBounds(getWidth() - 113, 5, 30, 30);
        size.setBounds(getWidth() - 73, 5, 30, 30);
        trigger.setBounds(getWidth() - 34, 5, 30, 30);
    }

    void setValue(ValueTree vt) {
        int targetSize = vt.getProperty(IDs::arraySize);
        size.setValue(targetSize, NotificationType::dontSendNotification);
        stepEditor.setValues(vt);
        stepEditor.setSize(targetSize);
        int ppqIndex = vt.getProperty(IDs::arrayPpq);
        primPpq.setSelectedItemIndex(ppqIndex, NotificationType::dontSendNotification);

        values = vt;
        repaint();
    }

    void setTrigger(ValueTree vt) {
        int targetNote = vt.getProperty(IDs::triggerMidi);
        trigger.setValue(targetNote, NotificationType::dontSendNotification);
        vtTrigger = vt;
    }
};


class PatternEditor : public Component {
    ComboBox presets;
    RowEditor rowEditors[8];
    TextButton active;
    ValueTree organPresets;
    ValueTree activePattern;

public:
    PatternEditor();
    void resized();
    void setTriggers(ValueTree vt);
    void setActivePattern(ValueTree vt);
};

#pragma once

#include <stdlib.h>
#include "PluginProcessor.h"
#include "layout.hpp"

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
        g.fillAll(Colours::black);

        int modulo = 4;

        for(int i=0;i<size;i++) {
            if ( values[i] != 0 )
                if ( i % modulo )
                    g.setColour(juce::Colours::orange);
                else
                    g.setColour(juce::Colours::darkorange);
            else
                if ( i % modulo )
                    g.setColour(juce::Colours::grey);
                else
                    g.setColour(juce::Colours::darkgrey);
            int x = i*ratio;

            g.fillRect( ratio*i , 0., ratio, ((float)getHeight()) );
        }
    }

    void mouseDown(const MouseEvent &event) override {
        if ( event.mods.isRightButtonDown() ) {
                getParentComponent()->mouseDown(event);
                return;
        }

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
    ComboBox secPpq;
    TextButton selectorPpq;
    Slider size;
    Slider trigger;
    Slider drift;
    TextButton muted;
    StepEditor stepEditor;
    ValueTree vtTrigger;
    TextButton action;

    void processAction();
public:
    ValueTree values;

    RowEditor() {
        addAndMakeVisible(stepEditor);
        addAndMakeVisible(size);
        addAndMakeVisible(primPpq);
        addAndMakeVisible(secPpq);
        addAndMakeVisible(selectorPpq);
        addAndMakeVisible(trigger);
        addAndMakeVisible(muted);
        addAndMakeVisible(action);

        primPpq.addItemList(PPQ_VALUES, 1);
        primPpq.onChange = [this] {
            values.setProperty(IDs::arrayPpqPrim, primPpq.getSelectedItemIndex(), nullptr);
        };

        secPpq.addItemList(PPQ_VALUES, 1);
        secPpq.onChange = [this] {
            values.setProperty(IDs::arrayPpqSec, secPpq.getSelectedItemIndex(), nullptr);
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
        muted.onClick = [this] {
            bool state = muted.getToggleState();
            values.setProperty(IDs::arrayMuted, state, nullptr);
        };

        action.setButtonText("A");
        action.onClick = [this] {
            processAction();
        };

        selectorPpq.setClickingTogglesState(true);
        selectorPpq.setButtonText("<");

        selectorPpq.onClick = [this] {
            bool state = selectorPpq.getToggleState();
            if ( state )
                selectorPpq.setButtonText(">");
            else
                selectorPpq.setButtonText("<");
            values.setProperty(IDs::arrayPpqActive, state, nullptr);
        };
    }

    void paint(juce::Graphics &g) {
        g.fillAll(Colours::darkgrey);
    }

    void resized() {
        SimpleRowLayout layout(0, 0, getWidth(), getHeight());

        layout.addToLeft(primPpq, 70);
        layout.addToLeft(selectorPpq, 30);
        layout.addToLeft(secPpq, 70);

        layout.addToRight(trigger, 30);
        layout.addToRight(size, 30);
        layout.addToRight(muted, 30);
        layout.addToRight(action, 30);
        layout.center(stepEditor);
    }

    void setValue(ValueTree vt) {
        int targetSize = vt.getProperty(IDs::arraySize);
        size.setValue(targetSize, NotificationType::dontSendNotification);
        stepEditor.setValues(vt);
        stepEditor.setSize(targetSize);
        muted.setToggleState(vt.getProperty(IDs::arrayMuted), NotificationType::dontSendNotification);
        int ppqIndex = vt.getProperty(IDs::arrayPpqPrim);
        primPpq.setSelectedItemIndex(ppqIndex, NotificationType::dontSendNotification);

        ppqIndex = vt.getProperty(IDs::arrayPpqSec);
        secPpq.setSelectedItemIndex(ppqIndex, NotificationType::dontSendNotification);

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
    TextButton presets;
    RowEditor rowEditors[8];
    TextButton active;
    ValueTree organPresets;
    ValueTree activePattern;

public:
    PatternEditor();
    void resized();
    void setTriggers(ValueTree vt);
    void setActivePattern(ValueTree vt);
    void processPreset();
};

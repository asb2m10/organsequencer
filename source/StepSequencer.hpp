#pragma once

#include "PluginProcessor.h"

const int NUM_SEQ = 8;

class Trigger {
    int note = 40;
    int velocity = 100;
    int panicnote = -1;
    int triggerPos = -1;
    int triggerOffPos = -1;
    int triggerLength = 4410;
public:
    void advance(juce::MidiBuffer& midiMessages, int s) {
        if ( panicnote >= 0 ) {
            midiMessages.addEvent(juce::MidiMessage(0x90, panicnote, 0), 0);
            panicnote = -1;
            triggerOffPos = -1;
        }

        if ( triggerPos >= 0 ) {
            midiMessages.addEvent(juce::MidiMessage(0x90, note, velocity), triggerPos);
            triggerPos = -1;
        }

        if ( triggerOffPos >= 0 ) {
            triggerOffPos -= s;
            if ( triggerOffPos <= 0 ) {
                midiMessages.addEvent(juce::MidiMessage(0x90, note, 0), s - triggerOffPos);
                triggerOffPos = -1;
            }
        }
    }

    void fire(int s) {
        fire(s, velocity);
    }

    void fire(int s, int v) {
        triggerPos = s;
        velocity = v;
        triggerOffPos = triggerPos + triggerLength;
    }

    void panic() {
        panicnote = note;
    }

    int setNote(int n) {
        panicnote = note;
        note = n;
    }

    void setTriggerLength(int l) {
        triggerLength = l;
    }
};

class StepSequencer {
    int value[64];
    int steps;
    bool muted;
public:
    void setStepSize(int s) {

    }

    int triggerPos(int ppq, int sampleRate) {
        return -1;
    }
};

class Pattern {
    bool muted;
public:
    StepSequencer seq[NUM_SEQ];

    void setMuted(bool m) {
        muted = m;
    }

    void triggerSeq(Trigger *triggers, int ppq, int sampleRate) {
        if ( muted )
            return;

        for(int i=0;i<NUM_SEQ;i++) {
            int pos = seq[i].triggerPos(ppq, sampleRate);

        }
    }
};

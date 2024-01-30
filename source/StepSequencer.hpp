#pragma once

#include "PluginProcessor.h"
#include "Model.hpp"
#include "trace.h"

String execute_jscript(ValueTree vt);

const int NUM_SEQ = 12;
const int NUM_PATTERN = 8;

//                                       0       1        2        3       4        5        6       7       8        9     10      11      12     13      14      15     16      17       18    19       20
const StringArray PPQ_VALUES    = { "1/128", "1/64", "1/32T", "1/64D", "1/32", "1/16T", "1/32D", "1/16", "1/8T", "1/16D", "1/8", "1/4T", "1/8D", "1/4", "1/2T", "1/4D", "1/2", "1/1T", "1/2D", "1/1", "1/1D" };
const float       PPQ_CORRESP[] = {   0.031250, 0.062500, 0.083333, 0.093750, 0.125000, 0.166667, 0.187500, 0.250000, 0.333333, 0.375000, 0.500000, 0.666667, 0.750000, 1.000000, 1.333333, 1.500000, 2.000000, 2.666667, 3.000000, 4.000000, 6.000000 };

class Trigger : public ValueTree::Listener {
    int velocity = 100;
    int panicnote = -1;
    int triggerPos = -1;
    int triggerOffPos = -1;
    int triggerLength = 4410;
public:
    ValueTree vtTrigger;
    int note = 0;

    Trigger() {
        vtTrigger = ValueTree(IDs::trigger);
        vtTrigger.addListener(this);
    }

    void advance(juce::MidiBuffer& midiMessages, int s) {
        if ( panicnote >= 0 ) {
            midiMessages.addEvent(juce::MidiMessage(0x90, panicnote, 0), 0);
            panicnote = -1;
            triggerOffPos = -1;
        }

        if ( triggerPos >= 0 ) {
            if ( triggerOffPos < 0 ) {
                midiMessages.addEvent(juce::MidiMessage(0x90, note, 0), triggerPos);
                triggerOffPos = -1;
            }
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

    void fire(int s, int dur) {
        fire(s, dur, velocity);
    }

    void fire(int s, int dur, int v) {
        triggerPos = s;
        velocity = v;
        if ( triggerOffPos == - 1)
            triggerOffPos = triggerPos + dur;
    }

    void panic() {
        panicnote = note;
    }

    void setNote(int n) {
        panicnote = note;
        note = n;
    }

    void setTriggerLength(int l) {
        triggerLength = l;
    }

    bool isActive() {
        return triggerOffPos > 0;
    }

    void valueTreePropertyChanged(ValueTree &tree, const Identifier &property) {
        if ( property != IDs::triggerMidi )
            return;

        int newNote = tree.getProperty(IDs::triggerMidi);
        setNote(newNote);
    }
};

class Sequencer {
    int sampleRate;
    double ppqWindow;
    double tm;

public:
    double samplePpq;

    void setSampleRate(int s) {
        sampleRate = s;
    }

    void setPos(float bpm, float t, int blockSize) {
        samplePpq = ( 60.0 / ( bpm ) ) * sampleRate;
        ppqWindow = ((double)blockSize) / samplePpq;
        tm = t;
    }

    void invalidatePos() {
        tm = -1;
    }

    int process(float eventlist[], int eventsize, float eventppq, int *epos) {
        if ( tm < 0 ) {
            *epos = 0;
            return -1;
        }

        double current = tm / eventppq;
        double next = (tm+ppqWindow) / eventppq;
        int nextPos = int(next);
        double target = ((double)nextPos) - current;
        *epos = nextPos % eventsize;

        if ( target >= 0 ) {
            if ( eventlist[*epos] > 0 )
                return target * eventppq * samplePpq;
            return -1;
        }

        return -1;
    }
};

class Pattern {
    juce::CachedValue<bool> softActive;

    struct ArrSeq : public ValueTree::Listener {
        ValueTree arraySeq;

        float values[64];
        float ppq = 1.0;
        bool muted = false;
        int size;
        int currentPos;
        int roll = 0;
        juce::CachedValue<int> recompute;

        ArrSeq() {
            arraySeq = ValueTree(IDs::ARRAYSEQ);
            arraySeq.setProperty(IDs::arrayValue, "0000000000000000", nullptr);
            arraySeq.setProperty(IDs::arrayPpqPrim, 13, nullptr);
            arraySeq.setProperty(IDs::arrayPpqSec, 13, nullptr);
            arraySeq.setProperty(IDs::arrayPpqActive, 0, nullptr);
            arraySeq.setProperty(IDs::arrayMuted, false, nullptr);
            arraySeq.setProperty(IDs::arrayCode, "", nullptr);
            arraySeq.setProperty(IDs::arrayRecompute, 0, nullptr);
            arraySeq.addListener(this);
            recompute.referTo(arraySeq, IDs::arrayRecompute, nullptr);
            for(int i=0;i<64;i++)
                values[i] = 0;
            size = 16;
        }

        void process(Trigger *trigger, Sequencer &seq) {
            int rollCheck = currentPos;
            int target = seq.process(values, size, ppq, &currentPos);
            if ( rollCheck > currentPos ) {
                if ( recompute != 0 ) {
                    roll++;
                }
            }

            if ( target >= 0  && !muted ) {
                trigger->fire(target, ppq * seq.samplePpq);
            }
        }

        void valueTreePropertyChanged(ValueTree &tree, const Identifier &property) {
            if ( property == IDs::arrayValue ) {
                String newValue = tree.getProperty(IDs::arrayValue).toString();
                size = newValue.length();
                jassert(size<64);
                int i;

                for(i=0;i<size;i++) {
                    if ( newValue[i] == '0' )
                        values[i] = 0;
                    else
                        values[i] = 1;
                }
                return;
            }

            if ( (property == IDs::arrayPpqPrim || property == IDs::arrayPpqSec ) || property == IDs::arrayPpqActive) {
                int active = tree.getProperty(IDs::arrayPpqActive);
                int pos;
                if ( active ) {
                    pos = tree.getProperty(IDs::arrayPpqSec);
                } else {
                    pos = tree.getProperty(IDs::arrayPpqPrim);
                }
                ppq = PPQ_CORRESP[pos];
                TRACE("setting ppq %f", ppq);
            }

            if ( property == IDs::arrayMuted ) {
                muted = tree.getProperty(IDs::arrayMuted);
            }
        }
    };
    ArrSeq arrseq[NUM_SEQ];

public:
    ValueTree value;
    bool active = false;

    Pattern() {
        value = ValueTree(IDs::pattern);
        for(int i=0;i<NUM_SEQ;i++) {
            value.addChild(arrseq[i].arraySeq, i, nullptr);
        }
        value.setProperty(IDs::patternSoftActive, false, nullptr);
        softActive.referTo(value, IDs::patternSoftActive, nullptr);
    }

    void process(Trigger trigger[], Sequencer &seq) {
        if ( !active && !softActive )
            return;

        for(int i=0;i<NUM_SEQ;i++) {
            arrseq[i].process(&trigger[i], seq);
        }
    }

    void updatePos(int *pos) {
        for(int i=0;i<NUM_SEQ;i++) {
            pos[i] = arrseq[i].currentPos;
        }
    }
};

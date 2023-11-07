#pragma once

#include "PluginProcessor.h"
#include "Model.hpp"

const int NUM_SEQ = 8;
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
    double samplePpq;
    double ppqWindow;
    double tm;
public:
    void setSampleRate(int s) {
        sampleRate = s;
    }

    void setPos(float bpm, float t, int blockSize) {
        samplePpq = ( 60.0 / ( bpm * 4 ) ) * sampleRate;
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
    juce::CachedValue<bool> muted;

    struct ArrSeq : public ValueTree::Listener {
        ValueTree arraySeq;

        float values[64];
        float ppq = 0.5;
        juce::CachedValue<int> size;
        bool muted = false;

        int currentPos;

        ArrSeq() {
            arraySeq = ValueTree(IDs::ARRAYSEQ);
            arraySeq.setProperty(IDs::arrayValue, "0000000000000000", nullptr);
            arraySeq.setProperty(IDs::arraySize, 16, nullptr);
            arraySeq.setProperty(IDs::arrayPpq, 13, nullptr);
            arraySeq.setProperty(IDs::arrayMuted, false, nullptr);
            size.referTo(arraySeq, IDs::arraySize, nullptr);
            arraySeq.addListener(this);
            for(int i=0;i<64;i++)
                values[i] = 0;
        }

        void process(Trigger *trigger, Sequencer &seq) {
            int pos;
            int target = seq.process(values, size, ppq, &currentPos);

            if ( target >= 0  && !muted ) {
                trigger->fire(target);
            }
        }

        void valueTreePropertyChanged(ValueTree &tree, const Identifier &property) {
            if ( property == IDs::arrayValue ) {
                String newValue = tree.getProperty(IDs::arrayValue).toString();
                int i;

                for(i=0;i<newValue.length();i++) {
                    if ( newValue[i] == '0' )
                        values[i] = 0;
                    else
                        values[i] = 1;
                }

                for(; i<size;i++) {
                    values[i] = 0;
                }
                return;
            }

            if ( property == IDs::arrayPpq ) {
                int pos = tree.getProperty(IDs::arrayPpq);
                ppq = PPQ_CORRESP[pos];
            }
        }
    };
    ArrSeq arrseq[NUM_SEQ];

public:
    ValueTree value;

    Pattern() {
        value = ValueTree(IDs::pattern);
        for(int i=0;i<8;i++) {
            value.addChild(arrseq[i].arraySeq, i, nullptr);
        }
        value.setProperty(IDs::patternMuted, true, nullptr);
        muted.referTo(value, IDs::patternMuted, nullptr);
    }

    bool isMuted() {
        return muted;
    }

    void process(Trigger trigger[], Sequencer &seq) {
        for(int i=0;i<NUM_SEQ;i++) {
            arrseq[i].process(&trigger[i], seq);
        }
    }
};

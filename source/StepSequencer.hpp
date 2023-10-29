#pragma once

#include "PluginProcessor.h"
#include "Model.hpp"

const int NUM_SEQ = 8;

class Trigger {
    int velocity = 100;
    int panicnote = -1;
    int triggerPos = -1;
    int triggerOffPos = -1;
    int triggerLength = 4410;
public:
    CachedValue<int> note;

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
        float values[64];
        juce::CachedValue<float> ppq;
        juce::CachedValue<int> size;
        bool muted = false;

        int currentPos;

        ArrSeq() {
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
            if ( property != IDs::arrayValue )
                return;

            String newValue = tree.getProperty(IDs::arrayValue).toString();
            for(int i=0;i<size;i++) {
                if ( newValue[i] == '0' )
                    values[i] = 0;
                else
                    values[i] = 1;
            }
        }
    };
    ArrSeq arrseq[NUM_SEQ];

public:
    ValueTree value;

    Pattern() {
        value = ValueTree(IDs::pattern);
        for(int i=0;i<8;i++) {
            ValueTree arraySeq(IDs::ARRAYSEQ);
            arraySeq.setProperty(IDs::arrayValue, "0000000000000000", nullptr);
            arraySeq.setProperty(IDs::arraySize, 16, nullptr);
            arraySeq.setProperty(IDs::arrayPpq, 1.0, nullptr);

            arrseq[i].ppq.referTo(arraySeq, IDs::arrayPpq, nullptr);
            arrseq[i].size.referTo(arraySeq, IDs::arraySize, nullptr);

            value.addListener(arrseq);
            value.addChild(arraySeq, i, nullptr);
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

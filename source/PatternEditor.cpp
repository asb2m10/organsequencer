#pragma once

#include "PatternEditor.hpp"
#include "ext/value_tree_debugger.h"

const Identifier PRESETNAMES[] = {
    "16 BEAT", "BALLAD", "BEGUINE", "BOSSA", "CHA-CHA", "DISCO", "MAMBO", "MARCH",
    "MERENGUE", "ROCK", "RUMBA", "SAMBA", "SHUFFLE", "SWING", "TANGO", "WALTZ"
};
const int PRESETNAMES_NUM = 16;

void addPresetVT(ValueTree &dest, Identifier id, StringArray sa) {
    ValueTree vt(id);
    int sz = sa[0].length();

    for(int i=0;i<8;i++) {
        ValueTree row = ValueTree(IDs::ARRAYSEQ);
        row.setProperty(IDs::arrayPpq, 13, nullptr);
        row.setProperty(IDs::arraySize, sz, nullptr);
        row.setProperty(IDs::arrayValue, sa[i], nullptr);
        vt.addChild(row, -1, nullptr);
    }
    dest.addChild(vt, -1, nullptr);
}

PatternEditor::PatternEditor() {
    for(int i=0;i<8;i++) {
        addAndMakeVisible(rowEditors[i]);
    }
    addAndMakeVisible(active);
    addAndMakeVisible(presets);
    active.setClickingTogglesState(true);
    active.setButtonText("ACTIVE");

    organPresets = ValueTree(IDs::ORGRANPRESETS);
    addPresetVT(organPresets, PRESETNAMES[0], StringArray {
        "0000000000000000",
        "0000000000000000",
        "0000000000000000",
        "0000000000000000", 

        "0000000000000000",
        "0000000000000000",
        "0000001000000100",
        "1000100010001000", 
    });

    for(int i=0;i<PRESETNAMES_NUM;i++) {
        presets.addItem(PRESETNAMES[i].toString(), i+1);
    }

    presets.onChange = [this] {
        ValueTree newPreset = organPresets.getChild(presets.getSelectedItemIndex());

        //jassert( newPreset.isValid() );

        if ( activePattern != nullptr ) {
            // activePattern->copyPropertiesFrom(newPreset, nullptr);
            // auto vd = new ValueTreeDebugger();
            // vd->setSource(&(activePattern));
        //    setActivePattern(activePattern);
        }
        //activePattern = newPreset;
    };
}

void PatternEditor::resized() {
    int ratio = (getHeight()-35) / 8;
    for(int i=0;i<8;i++) {
        rowEditors[i].setBounds(2, (i * ratio) + 30, getWidth() - 2, ratio-5);
    }

    presets.setBounds(getWidth() - 300, 2, 220, 20);
    active.setBounds(2, 2, 80, 20);
}

void PatternEditor::setTriggers(ValueTree vt) {
    for(int i=0;i<8;i++) {
        rowEditors[i].setTrigger(vt.getChild(i));
    }
}

void PatternEditor::setActivePattern(ValueTree *vt) {
    activePattern = vt;

    for(int i=0;i<8;i++) {
        rowEditors[i].setValue(activePattern->getChild(i));
    }
}
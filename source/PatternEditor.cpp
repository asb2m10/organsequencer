#pragma once

#include "PatternEditor.hpp"

const StringArray PRESETNAMES = {
    "16 BEAT", "BALLAD", "BEGUINE", "BOSSA", "CHA-CHA", "DISCO", "MAMBO", "MARCH",
    "MERENGUE", "ROCK", "RUMBA", "SAMBA", "SHUFFLE", "SWING", "TANGO", "WALTZ"
};

PatternEditor::PatternEditor() {
    for(int i=0;i<8;i++) {
        addAndMakeVisible(rowEditors[i]);
    }
    addAndMakeVisible(active);
    addAndMakeVisible(presets);
    active.setClickingTogglesState(true);
    active.setButtonText("ACTIVE");    
}

void PatternEditor::resized() {
    int ratio = (getHeight()-35) / 8;
    for(int i=0;i<8;i++) {
        rowEditors[i].setBounds(2, (i * ratio) + 30, getWidth(), ratio-5);
    }

    presets.setBounds(getWidth() - 300, 2, 220, 20);
    active.setBounds(2, 2, 80, 20);    
}

void PatternEditor::setTriggers(ValueTree vt) {
    for(int i=0;i<8;i++) {
        rowEditors[i].setTrigger(vt.getChild(i));
    }
}

void PatternEditor::setActivePattern(ValueTree vt) {
    for(int i=0;i<8;i++) {
        rowEditors[i].setValue(vt.getChild(i));
    }
}
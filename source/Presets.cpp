#pragma once

#include "PatternEditor.hpp"

const Identifier PRESETNAMES[] = {
    "16 BEAT", "BALLAD", "BEGUINE", "BOSSA", "CHA-CHA", "DISCO", "MAMBO", "MARCH",
    "MERENGUE", "ROCK", "RUMBA", "SAMBA", "SHUFFLE", "SWING", "TANGO", "WALTZ"
};
const int PRESETNAMES_NUM = 16;

struct PresetDef {
    StringArray values;
    float ppq;
};

struct PresetContainer {
    PresetDef presets[PRESETNAMES_NUM];

    PresetContainer() {
        presets[0].values = StringArray { // 16 BEAT
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0111011101010111",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000001000001000",
            "1000000010100000",
        };
        presets[0].ppq = 7;

        presets[1].values = StringArray { // BALLAD
            "000000000000",
            "000000000000",
            "000000000000",
            "000000000000",
            "000000000000",
            "001010001000",
            "000000000000",
            "000000000000",
            "000000000000",
            "000000100000",
            "000000000000",
            "100000000010",
        };
        presets[1].ppq = 7;

        presets[2].values = StringArray { // BEGUINE
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0101001001010010",
            "0010010100100101",
            "1000100010001000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "1001001000101000",
            "0000000000000000",
        };
        presets[2].ppq = 7;

        presets[3].values = StringArray { // BOSSA
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0100001101100101",
            "0000000000000000",
            "0000000000000000",
            "0010010010010010",
            "0000000000000000",
            "0000000000000000",
            "1001100010011000",
        };
        presets[3].ppq = 7;

        presets[4].values = StringArray { // CHA-CHA
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "1000100010000000",
            "0010001000100000",
            "0000000000001010",
            "0000000000000000",
            "1000100010101000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
        };
        presets[4].ppq = 7;

        presets[5].values = StringArray { // DISCO
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0010001000100010",
            "0000000000000000",
            "0101010101010101",
            "0000000000000000",
            "0000100000001000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "1000100010001000",
        };
        presets[5].ppq = 7;

        presets[6].values = StringArray { // MAMBO
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000001010000000",
            "0010000000100000",
            "0000000000001010",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "1000100010001000",
        };
        presets[6].ppq = 7;

        presets[7].values = StringArray { // MARCH
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "1010000000100000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000101010001010",
            "0000000000000000",
            "1000100010001000",
        };
        presets[7].ppq = 7;

        presets[8].values = StringArray { // MERENGUE
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0010001000000000",
            "1000100010101010",
            "0000000000001000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0010001000000000",
            "1000000010000000",
        };
        presets[8].ppq = 7;

        presets[9].values = StringArray { // ROCK
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0010000000100010",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000100000001000",
            "0000000000000000",
            "1000001010000000",
        };
        presets[9].ppq = 7;

        presets[10].values = StringArray { // RUMBA
            "0000000000000000",
            "0000000000001000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000100000100010",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0010000010000000",
            "1000001000001000",
            "1000001010000000",
        };
        presets[10].ppq = 7;

        presets[11].values = StringArray { // SAMBA
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "1010000101010000",
            "0101011010100101",
            "0000100000001010",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "1000000010000000",
        };
        presets[11].ppq = 7;

        presets[12].values = StringArray { // SHUFFLE
            "000000000000",
            "100000100000",
            "000000000000",
            "000000000000",
            "000000000000",
            "000101000101",
            "000000000000",
            "000000000000",
            "000000000000",
            "000000100000",
            "000000000000",
            "100000000000",
        };
        presets[12].ppq = 7;

        presets[13].values = StringArray { // SWING
            "000000000000",
            "100000000000",
            "000000000000",
            "000000000000",
            "000000000000",
            "000000100010",
            "000000000000",
            "000000000000",
            "000000000000",
            "000000100000",
            "000000000000",
            "100000000000",
        };
        presets[13].ppq = 7;

        presets[14].values = StringArray { // TANGO
            "0000000000000000",
            "0010000010000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000001000100000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "1000100010001011",
            "0000000000000000",
            "1000000000000000",
        };
        presets[14].ppq = 7;

        presets[15].values = StringArray { // WALTZ
            "000000000000",
            "000000000000",
            "000000000000",
            "000000000000",
            "000000000000",
            "000000000000",
            "000000000000",
            "000000000000",
            "000000000000",
            "000010001000",
            "000000000000",
            "100000000000",
        };
        presets[15].ppq = 7;
    }
};

PresetContainer presetContainer;

void PatternEditor::processPreset() {
    PopupMenu m;
    m.addItem ("Clear", [this]() {
        for(int i=0;i<NUM_SEQ;i++) {
            String orginalValue = rowEditors[i].values.getProperty(IDs::arrayValue);
            String newValue = String::repeatedString("0", orginalValue.length());
            rowEditors[i].values.setProperty(IDs::arrayValue, newValue, nullptr);
        }

    });
    m.addItem ("Copy", [this]() { TRACE("OK"); });
    m.addItem ("Paste", [this]() { TRACE("OK"); });
    m.addSeparator();
    for(int i=0;i<PRESETNAMES_NUM;i++) {
        m.addItem(i+1, PRESETNAMES[i].toString(), true, false);
    }
    m.showMenuAsync(PopupMenu::Options().withDeletionCheck(*this).withMousePosition(), [this](int item) {
        item = item - 1;
        if ( item < 0 )
            return;
        for(int i=0;i<NUM_SEQ;i++) {
            rowEditors[i].values.setProperty(IDs::arrayPpqPrim, presetContainer.presets[item].ppq, nullptr);
            rowEditors[i].values.setProperty(IDs::arrayPpqActive, 0, nullptr);
            rowEditors[i].values.setProperty(IDs::arrayValue, presetContainer.presets[item].values[i], nullptr);
            rowEditors[i].refresh();
        }
    });
}

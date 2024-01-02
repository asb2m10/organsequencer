#pragma once

#include "PatternEditor.hpp"
#include "ext/value_tree_debugger.h"

const Identifier PRESETNAMES[] = {
    "16 BEAT", "BALLAD", "BEGUINE", "BOSSA", "CHA-CHA", "DISCO", "MAMBO", "MARCH",
    "MERENGUE", "ROCK", "RUMBA", "SAMBA", "SHUFFLE", "SWING", "TANGO", "WALTZ"
};
const int PRESETNAMES_NUM = 16;



class PatternTool : public Component {
    TextEditor script;
    ValueTree content;
    TextButton shiftLeft;
    TextButton shiftRight;
    TextButton clear;
    TextButton addItem;
    TextButton removeItem;

public:
    PatternTool(ValueTree vt) {
        content = vt;
        addAndMakeVisible(script);
        addAndMakeVisible(shiftLeft);
        shiftLeft.setButtonText("<<");
        addAndMakeVisible(shiftRight);
        shiftRight.setButtonText(">>");
        addAndMakeVisible(clear);
        clear.setButtonText("Clear");
        addAndMakeVisible(addItem);
        addItem.setButtonText("+");
        addAndMakeVisible(removeItem);
        removeItem.setButtonText("-");
    }

    void resized() override {

    }
};

void addPresetVT(ValueTree &dest, Identifier id, StringArray sa) {
    ValueTree vt(id);
    int sz = sa[0].length();

    for(int i=0;i<8;i++) {
        ValueTree row = ValueTree(IDs::ARRAYSEQ);
        row.setProperty(IDs::arrayPpqPrim, 13, nullptr);
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

    active.onClick = [this] {
                  auto colourSelector = std::make_unique<ColourSelector>();

            colourSelector->setName ("background");
            colourSelector->setCurrentColour (findColour (TextButton::buttonColourId));
            colourSelector->setColour (ColourSelector::backgroundColourId, Colours::transparentBlack);
            colourSelector->setSize (300, 400);

            CallOutBox::launchAsynchronously (std::move (colourSelector), rowEditors[7].getScreenBounds(), nullptr);
        

    };

    presets.setButtonText("Preset");
    presets.onClick = [this] {
        this->processPreset();
    };

    organPresets = ValueTree(IDs::ORGRANPRESETS);
    addPresetVT(organPresets, PRESETNAMES[0], StringArray {
        "1111111111111111",
        "0000000000000000",
        "0000000000000000",
        "0000000000000000",
        "0000000000000000",
        "0000000000000000",
        "0000001000000100",
        "1000100010001000",
    });

    /*presets.onChange = [this] {
        ValueTree newPreset = organPresets.getChild(presets.getSelectedItemIndex());

        //jassert( newPreset.isValid() );

        //if ( activePattern != nullptr ) {
            // activePattern->copyPropertiesFrom(newPreset, nullptr);
            // auto vd = new ValueTreeDebugger();
            // vd->setSource(&(activePattern));
        //    setActivePattern(activePattern);
        //}
        //activePattern = newPreset;
    };*/
}

void PatternEditor::resized() {
    int ratio = (getHeight()-25) / 8;
    for(int i=0;i<8;i++) {
        rowEditors[i].setBounds(2, (i * ratio) + 25, getWidth() - 2, ratio-5);
    }

    presets.setBounds(getWidth() - 114, 2, 110, 20);
    active.setBounds(2, 2, 80, 20);
}

void PatternEditor::setTriggers(ValueTree vt) {
    for(int i=0;i<8;i++) {
        rowEditors[i].setTrigger(vt.getChild(i));
    }
}

void PatternEditor::setActivePattern(ValueTree vt) {
    activePattern = vt;

    for(int i=0;i<8;i++) {
        rowEditors[i].setValue(activePattern.getChild(i));
    }
}

void RowEditor::processAction() {
    PopupMenu m;
    m.addItem ("<< Shift Left", [this]() { TRACE("OK"); });
    m.addItem (">> Shift Right", [this]() { TRACE("OK"); });
    m.addSeparator();
    m.addItem ("Clear", [this]() { TRACE("OK"); });
    m.addItem ("Invert", [this]() { TRACE("OK"); });
    m.showMenuAsync(PopupMenu::Options().withDeletionCheck(*this).withMousePosition());
}

void PatternEditor::processPreset() {
    PopupMenu m;
    m.addItem ("Clear", [this]() {
        for(int i=0;i<8;i++) {

        }

    });
    m.addItem ("Copy", [this]() { TRACE("OK"); });
    m.addItem ("Paste", [this]() { TRACE("OK"); });
    m.addSeparator();
    for(int i=0;i<PRESETNAMES_NUM;i++) {
        m.addItem(i+1, PRESETNAMES[i].toString(), true, false);
    }
    m.showMenuAsync(PopupMenu::Options().withDeletionCheck(*this).withMousePosition(), [this](int item) {
        ValueTree vt = organPresets.getChild(item-1);
        if ( ! vt.isValid() )
            return;

        for(int i=0;i<8;i++) {
            rowEditors[i].values.setProperty(IDs::arrayPpqPrim, vt.getChild(i).getProperty(IDs::arrayPpqPrim), nullptr);
            rowEditors[i].values.setProperty(IDs::arrayPpqActive, 0, nullptr);
            rowEditors[i].values.setProperty(IDs::arraySize, vt.getChild(i).getProperty(IDs::arraySize), nullptr);
            rowEditors[i].values.setProperty(IDs::arrayValue, vt.getChild(i).getProperty(IDs::arrayValue), nullptr);
            rowEditors[i].repaint();
        }
    });
}
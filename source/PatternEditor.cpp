#pragma once

#include "PatternEditor.hpp"
#include "ext/value_tree_debugger.h"

const Identifier PRESETNAMES[] = {
    "16 BEAT", "BALLAD", "BEGUINE", "BOSSA", "CHA-CHA", "DISCO", "MAMBO", "MARCH",
    "MERENGUE", "ROCK", "RUMBA", "SAMBA", "SHUFFLE", "SWING", "TANGO", "WALTZ"
};
const int PRESETNAMES_NUM = 16;

class PatternAction : public Component {
    TextEditor script;
    ValueTree content;
    TextButton shiftLeft;
    TextButton shiftRight;
    TextButton clear;
    TextButton invert;
    TextButton addItem;
    TextButton removeItem;

    TextButton runScript;
    ComboBox reprocessScipt;

public:
    PatternAction(ValueTree vt) {
        content = vt;
        addAndMakeVisible(script);
        script.setMultiLine(true, false);
        script.setReturnKeyStartsNewLine(true);
        addAndMakeVisible(shiftLeft);
        shiftLeft.setButtonText("<<");
        shiftLeft.onClick = [this] {
            String src = content.getProperty(IDs::arrayValue);
            String end = src.substring(1);
            String dest = end + src[0];
            content.setProperty(IDs::arrayValue, dest, nullptr);
        };

        addAndMakeVisible(shiftRight);
        shiftRight.setButtonText(">>");
        shiftRight.onClick = [this] {
            String src = content.getProperty(IDs::arrayValue);
            String start = src.dropLastCharacters(1);
            content.setProperty(IDs::arrayValue, start + src, nullptr);
        };

        addAndMakeVisible(clear);
        clear.setButtonText("Clear");
        clear.onClick = [this] {
            String target = content.getProperty(IDs::arrayValue);
            target = String::repeatedString("0", target.length());
            content.setProperty(IDs::arrayValue, target, nullptr);
        };

        addAndMakeVisible(invert);
        invert.setButtonText("Invert");

        addAndMakeVisible(addItem);
        addItem.setButtonText("+");

        addAndMakeVisible(removeItem);
        removeItem.setButtonText("-");

        addAndMakeVisible(runScript);
        runScript.setButtonText("RUN");
        runScript.onClick = [this] {
            content.setProperty(IDs::arrayCode, script.getText(), nullptr);
            execute_jscript(content);
        };

        addAndMakeVisible(reprocessScipt);
        reprocessScipt.addItem("NEVER", 1);
        reprocessScipt.addItem("1 BAR", 2);
        reprocessScipt.addItem("2 BAR", 3);
    }

    void resized() override {
        SimpleRowLayout top(0, 0, getWidth(), 40);

        top.addToLeft(shiftLeft, 40);
        top.addToLeft(shiftRight, 40);
        top.addToLeft(addItem, 40);
        top.addToLeft(removeItem, 40);

        top.addToRight(invert, 50);
        top.addToRight(clear, 50);

        SimpleRowLayout bottom(0, getHeight() - 45, getWidth(), 40);
        bottom.addToRight(runScript, 60);
        bottom.addToRight(reprocessScipt, 150);

        script.setBounds(5, 45, getWidth() - 10, getHeight() - 45 - 45 - 10);
    }
private:
};

PatternEditor::PatternEditor() {
    for(int i=0;i<8;i++) {
        addAndMakeVisible(rowEditors[i]);
    }
    addAndMakeVisible(active);
    addAndMakeVisible(presets);
    active.setClickingTogglesState(true);
    active.setButtonText("ACTIVE");

    active.onClick = [this] {


    };

    presets.setButtonText("Preset");
    presets.onClick = [this] {
        this->processPreset();
    };
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
    auto patternAction = std::make_unique<PatternAction>(values);
    patternAction->setSize (400, 200);
    CallOutBox::launchAsynchronously (std::move (patternAction), action.getScreenBounds(), nullptr);    
}


struct PresetDef {
    StringArray values;
    float ppq;
};

struct PresetContainer {
    PresetDef presets[PRESETNAMES_NUM];

    PresetContainer() {
        presets[0].values = StringArray {
            "0000000000000000",
            "0111011101010111",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000001000001000",
            "1000000010100000",
        };
        presets[0].ppq = 13;
        presets[1].values = StringArray {
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
        };
        presets[1].ppq = 13;
        presets[2].values = StringArray {
            "0101001001010010",
            "0010010100100101",
            "1000100010001000",
            "0000000000000000",
            "0000000000000000",
            "0000000000000000",
            "1001001000101000",
            "0000000000000000",
        };
        presets[2].ppq = 13;
    }
};

PresetContainer presetContainer;

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
        item = item - 1;
        for(int i=0;i<8;i++) {
            rowEditors[i].values.setProperty(IDs::arrayPpqPrim, presetContainer.presets[item].ppq, nullptr);
            rowEditors[i].values.setProperty(IDs::arrayPpqActive, 0, nullptr);
            rowEditors[i].values.setProperty(IDs::arraySize, presetContainer.presets[item].values[i].length(), nullptr);
            rowEditors[i].values.setProperty(IDs::arrayValue, presetContainer.presets[item].values[i], nullptr);
            rowEditors[i].refresh();
        }
    });
}


String execute_jscript(ValueTree vt) {
    JavascriptEngine engine;
    Identifier seqID("seq");

    String script = vt.getProperty(IDs::arrayCode);

    Result first = engine.execute(script);
    if ( first.failed() ) {
        TRACE("First error %s", first.getErrorMessage().toRawUTF8());
        return first.getErrorMessage();
    }

    auto ret = engine.getRootObjectProperties();

    if ( ret.contains("seq") ) {
        var seq = ret[seqID];

        auto seqv = seq.getArray();
        // for(int i=0;i++, seqv->size(); i++) {
        //     int v = seqv->getFirst();
        // }
        
    }

    return "";
}
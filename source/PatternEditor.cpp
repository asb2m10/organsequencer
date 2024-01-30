#pragma once

#include "PatternEditor.hpp"
#include "ext/value_tree_debugger.h"

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
    ComboBox reprocessScript;

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
            String start = src.substring(0, src.length()-1);
            String target = src[src.length()-1] + start;
            content.setProperty(IDs::arrayValue, target, nullptr);
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
        invert.onClick = [this] {
            String src = content.getProperty(IDs::arrayValue);
            const char *srca = src.toRawUTF8();
            char target[65];
            int i;
            for(i=0;i<src.length();i++)
                target[i] = srca[i] == '1' ? '0' : '1';
            target[i] = 0;
            content.setProperty(IDs::arrayValue, String(target), nullptr);
        };

        addAndMakeVisible(addItem);
        addItem.setButtonText("+");
        addItem.onClick = [this] {
            Random random;
            String src = content.getProperty(IDs::arrayValue);
            const char *srca = src.toRawUTF8();
            char target[65];
            int test[64];
            int i, j;
            for(i=0,j=0;i<src.length();i++) {
                target[i] = srca[i];
                if ( srca[i] == '0' ) {
                    test[++j] = i;
                }
            }
            j--;
            if ( j == 0 )
                return;
            target[i] = 0;
            int r = random.nextInt(j);
            int newItem = test[r];
            TRACE("adding to %d %d %d", r, j, newItem);
            target[newItem] = '1';
            content.setProperty(IDs::arrayValue, String(target), nullptr);
        };

        addAndMakeVisible(removeItem);
        removeItem.setButtonText("-");

        addAndMakeVisible(runScript);
        runScript.setButtonText("RUN");
        runScript.onClick = [this] {
            content.setProperty(IDs::arrayCode, script.getText(), nullptr);
            execute_jscript(content);
        };

        addAndMakeVisible(reprocessScript);
        reprocessScript.addItem("NEVER", 1);
        reprocessScript.addItem("1 BAR", 2);
        reprocessScript.addItem("2 BAR", 3);
        reprocessScript.addItem("4 BAR", 4);
        reprocessScript.setSelectedItemIndex(0);
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
        bottom.addToRight(reprocessScript, 150);

        script.setBounds(5, 45, getWidth() - 10, getHeight() - 45 - 45 - 10);
    }
};

PatternEditor::PatternEditor() {
    for(int i=0;i<NUM_SEQ;i++) {
        addAndMakeVisible(rowEditors[i]);
    }
    addAndMakeVisible(active);
    addAndMakeVisible(presets);
    active.setClickingTogglesState(true);
    active.setButtonText("ACTIVE");

    active.onClick = [this] {
        activePattern.setProperty(IDs::patternSoftActive, active.getToggleState(), nullptr);
    };

    presets.setButtonText("Preset");
    presets.onClick = [this] {
        this->processPreset();
    };
}

void PatternEditor::resized() {
    int ratio = (getHeight()-25) / NUM_SEQ;
    for(int i=0;i<NUM_SEQ;i++) {
        rowEditors[i].setBounds(2, (i * ratio) + 25, getWidth() - 2, ratio-5);
    }

    presets.setBounds(getWidth() - 114, 2, 110, 20);
    active.setBounds(2, 2, 80, 20);
}

void PatternEditor::setTriggers(ValueTree vt) {
    for(int i=0;i<NUM_SEQ;i++) {
        rowEditors[i].setTrigger(vt.getChild(i));
    }
}

void PatternEditor::setActivePattern(ValueTree vt) {
    activePattern = vt;

    for(int i=0;i<NUM_SEQ;i++) {
        rowEditors[i].setValue(activePattern.getChild(i));
    }
    bool isActive = activePattern.getProperty(IDs::patternSoftActive);
    active.setToggleState(isActive, false);
}

void RowEditor::processAction() {
    auto patternAction = std::make_unique<PatternAction>(values);
    patternAction->setSize (450, 300);
    CallOutBox::launchAsynchronously(std::move (patternAction), action.getScreenBounds(), nullptr);
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
    }

    return "";
}
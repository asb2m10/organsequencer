#pragma once

#include "PatternEditor.hpp"
#include "ext/value_tree_debugger.h"
#include "ext/JavascriptCodeTokeniser.h"

class PatternAction : public Component {
    CodeDocument codeDocument;
    //TextEditor script;
    CodeEditorComponent script;
    ValueTree content;
    TextButton shiftLeft;
    TextButton shiftRight;
    TextButton clear;
    TextButton invert;
    TextButton addItem;
    TextButton removeItem;

    TextButton runScript;
    ComboBox presetScripts;
    ComboBox reprocessScript;
    TextButton showLocation;
    TextButton saveScript;
    JavascriptTokeniser tokenizer;

public:
    PatternAction(ValueTree vt) : script(codeDocument, nullptr) {
        content = vt;
        addAndMakeVisible(script);
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
            std::vector<int> posi;

            for(int i=0;i<src.length();i++) {
                if ( src[i] != '1' )
                    posi.push_back(i);
            }

            if ( posi.size() == 0 )
                return;

            int r = random.nextInt(posi.size());
            content.setProperty(IDs::arrayValue, src.replaceSection(posi[r], 1, "1"), nullptr);
        };

        addAndMakeVisible(removeItem);
        removeItem.setButtonText("-");
        removeItem.onClick = [this] {
            Random random;
            String src = content.getProperty(IDs::arrayValue);
            std::vector<int> posi;

            for(int i=0;i<src.length();i++) {
                if ( src[i] == '1' )
                    posi.push_back(i);
            }

            if ( posi.size() == 0 )
                return;

            int r = random.nextInt(posi.size());
            content.setProperty(IDs::arrayValue, src.replaceSection(posi[r], 1, "0"), nullptr);
        };

        addAndMakeVisible(runScript);
        runScript.setButtonText("RUN");
        runScript.onClick = [this] {
            content.setProperty(IDs::arrayCode, codeDocument.getAllContent(), nullptr);
            execute_jscript(content);
        };

        addAndMakeVisible(reprocessScript);
        reprocessScript.addItem("NEVER", 1);
        reprocessScript.addItem("1 BAR", 2);
        reprocessScript.addItem("2 BAR", 3);
        reprocessScript.addItem("4 BAR", 4);
        reprocessScript.setSelectedItemIndex(0);

        addAndMakeVisible(presetScripts);
        showLocation.setButtonText("Show Location");
        addAndMakeVisible(showLocation);
        saveScript.setButtonText("Save");
        addAndMakeVisible(saveScript);
    }

    void resized() override {
        SimpleRowLayout top(0, 0, getWidth(), 40);

        top.addToLeft(shiftLeft, 40);
        top.addToLeft(shiftRight, 40);
        top.addToLeft(addItem, 40);
        top.addToLeft(removeItem, 40);

        top.addToRight(invert, 50);
        top.addToRight(clear, 50);

        SimpleRowLayout preset(0, getHeight() - 45 - 45, getWidth(), 40);
        preset.addToRight(presetScripts, 200);
        preset.addToRight(saveScript, 60);
        preset.addToLeft(showLocation, 140);

        SimpleRowLayout bottom(0, getHeight() - 45, getWidth(), 40);
        bottom.addToRight(runScript, 60);
        bottom.addToRight(reprocessScript, 200);

        script.setBounds(5, 45, getWidth() - 10, getHeight() - 45 - 45 - 45 - 10);
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
    patternAction->setSize (550, 350);
    CallOutBox::launchAsynchronously(std::move (patternAction), action.getScreenBounds(), nullptr);
}

Identifier seqID("seq");

void exec_internal(JavascriptEngine &engine, String script) {
    Result rc = engine.execute(script);

    if ( rc.failed() ) {
        TRACE("EXCEPTION %s" , rc.getErrorMessage().toRawUTF8());
        throw rc.getErrorMessage();
    }
}

String execute_jscript(ValueTree vt) {
    JavascriptEngine engine;
    String script = vt.getProperty(IDs::arrayCode);

    try {
        String ppqValue = vt.getProperty(IDs::arrayPpqPrim);
        String ppqInject = String("ppq = ") + String(ppqValue);
        exec_internal(engine, ppqInject);
        exec_internal(engine, script);

        auto ret = engine.getRootObjectProperties();

        if ( ret.contains("seq") ) {
            var seq = ret[seqID];

            Array<juce::var> *seqarr = seq.getArray();

            if ( seqarr != nullptr ) {
                StringArray newArray;
                for(int i=0;i<seqarr->size();i++) {
                    int v = (*seqarr)[i];
                    if ( v == 0 ) {
                        newArray.add("0");
                    } else {
                        newArray.add("1");
                    }
                }

                if ( newArray.size() != 0 )
                    vt.setProperty(IDs::arrayValue, newArray.joinIntoString(""), nullptr);
            }
        }
    } catch (String str) {
        return str;
    }
    return "";
}
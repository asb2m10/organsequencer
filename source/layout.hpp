#pragma once

#include "PluginProcessor.h"

class SimpleRowLayout {
    int x;
    int y;
    int w;
    int h;

    int sl = 0;
    int sr = 0;
public:
    SimpleRowLayout(int startx, int starty, int width, int height, int gapx = 2, int gapy = 5) {
        x = startx + gapx;
        y = starty + gapy;
        w = width - gapx*2;
        h = height - gapy*2;
    }

    void addToLeft(Component &c, int size, int space = 5) {
        c.setBounds(x+sl+space, y, size, h);
        sl += space + size;
    }

    void addToRight(Component &c, int size, int space = 5) {
        c.setBounds(w - sr - size - space, y, size, h);
        sr += space + size;
    }

    void center(Component &c, int space = 5) {
        c.setBounds(x+sl+space, y,  (w - sr - space) - (x+sl+space), h);
    }
};

class SimpleColumnLayout {
    int x;
    int y;
    int w;
    int h;

    int st = 0;
    int sb = 0;
public:
    SimpleColumnLayout(int startx, int starty, int width, int height, int gapx = 2, int gapy = 5) {
        x = startx + gapx;
        y = starty + gapy;
        w = width - gapx*2;
        h = height - gapy*2;
    }

    void addToTop(Component &c, int size, int space = 5) {
        c.setBounds(x+st+space, y, size, h);
        c.setBounds(x, y+st+space, w, size);

        st += space + size;
    }

    void addToBottom(Component &c, int size, int space = 5) {
        c.setBounds(x, h - sb - size - space, w, size);
        sb += space + size;
    }

    void center(Component &c, int space = 5) {
        c.setBounds(x+st+space, y,  (w - sb - space) - (x+st+space), h);
    }
};

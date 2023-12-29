#pragma once

#include <JuceHeader.h>

namespace IDs
{
#define DECLARE_ID(name) const juce::Identifier name (#name);
    DECLARE_ID(ROOT)
    DECLARE_ID(bpm)
    DECLARE_ID(internalSeq)
    DECLARE_ID(TRIGGERS)
    DECLARE_ID(trigger)
    DECLARE_ID(triggerMidi)
    DECLARE_ID(PATTERNS)
    DECLARE_ID(pattern)
    DECLARE_ID(patternMuted)
    DECLARE_ID(ARRAYSEQ)
    DECLARE_ID(arrayValue)
    DECLARE_ID(arrayDrift)
    DECLARE_ID(arraySize)
    DECLARE_ID(arrayPpq)
    DECLARE_ID(arrayPpqPrim)
    DECLARE_ID(arrayPpqSec)
    DECLARE_ID(arrayMuted)
    DECLARE_ID(ORGRANPRESETS)
#undef DECLARE_ID
}
cmake_minimum_required(VERSION 3.15)

project(ORGANSEQ VERSION 0.0.1)
add_definitions(-w)
add_subdirectory(libs/JUCE)

juce_add_plugin(OrganSeq
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "Digital Suburban"             # Specify the name of the plugin's author
    BUNDLE_ID "digitalsuburban"
    # IS_SYNTH TRUE/FALSE                       # Is this a synth or an effect?
    NEEDS_MIDI_INPUT TRUE                       # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT TRUE                      # Does the plugin need midi output?
    IS_MIDI_EFFECT TRUE                         # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE                # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE DGSB               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE ORGS                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS VST3 Standalone                     # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "OrganSequencer")           # The name of the final executable, which can differ from the target name


juce_generate_juce_header(OrganSeq)


target_sources(OrganSeq
    PRIVATE
        source/PluginEditor.cpp
        source/PluginProcessor.cpp
        source/PatternEditor.cpp
        source/Presets.cpp
        source/ext/value_tree_debugger.cpp
        source/ext/JavascriptCodeTokeniser.cpp)


target_compile_definitions(OrganSeq
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(OrganSeq
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

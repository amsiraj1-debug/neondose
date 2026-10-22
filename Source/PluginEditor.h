#pragma once
#include "PluginProcessor.h"
#include "StyleLibrary.h"

class NeonDoseAudioProcessorEditor final : public juce::AudioProcessorEditor,
                                            private juce::ListBoxModel {
public:
    explicit NeonDoseAudioProcessorEditor(NeonDoseAudioProcessor&);
    ~NeonDoseAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    int getNumRows() override;
    void paintListBoxItem(int row, juce::Graphics&, int width, int height, bool selected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void filterPresets();
    void loadPreset(int index);
    void buildPrompt();
    void selectVariation(int kind);
    void updateStatus(const juce::String&);

    NeonDoseAudioProcessor& processor;
    std::vector<nd::Style> styles;
    std::vector<int> visibleStyles;
    std::array<juce::Slider, 8> dials;
    std::array<juce::Label, 8> dialNames;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 8> attachments;
    juce::ComboBox categories;
    juce::TextEditor search, prompt;
    juce::ListBox presetList;
    juce::TextButton applyButton, softButton, wideButton, edgeButton;
    juce::Label heroTitle, heroSubtitle, status, sectionLibrary, sectionPrompt, note;
    std::array<float, 8> promptValues{};
    bool havePrompt = false;
    std::unique_ptr<juce::LookAndFeel_V4> nativeLaf;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeonDoseAudioProcessorEditor)
};

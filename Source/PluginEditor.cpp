#include "PluginEditor.h"
#include "PromptEngine.h"
#include <cmath>

namespace {
const juce::Colour pink = juce::Colour::fromRGB(255, 103, 186);
const juce::Colour faint = juce::Colour::fromRGB(222, 162, 201);
const juce::Colour ink = juce::Colour::fromRGB(19, 9, 23);
const juce::Colour panel = juce::Colour::fromRGB(31, 20, 36);
const juce::Colour cream = juce::Colour::fromRGB(242, 235, 242);
const char* ids[] = { "drive", "tone", "air", "comp", "space", "delay", "mix", "output" };
const char* labels[] = { "DRIVE", "TONE", "AIR", "COMP", "SPACE", "DELAY", "MIX", "OUTPUT" };

struct NeonLook final : public juce::LookAndFeel_V4 {
    NeonLook() {
        setColour(juce::TextEditor::backgroundColourId, panel);
        setColour(juce::TextEditor::textColourId, cream);
        setColour(juce::TextEditor::outlineColourId, pink.withAlpha(0.32f));
        setColour(juce::TextEditor::focusedOutlineColourId, pink);
        setColour(juce::TextEditor::highlightColourId, pink.withAlpha(0.30f));
        setColour(juce::ComboBox::backgroundColourId, panel);
        setColour(juce::ComboBox::textColourId, cream);
        setColour(juce::ComboBox::outlineColourId, pink.withAlpha(0.35f));
        setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(92, 38, 76));
        setColour(juce::TextButton::buttonOnColourId, pink);
        setColour(juce::TextButton::textColourOffId, cream);
        setColour(juce::ListBox::backgroundColourId, panel);
        setColour(juce::ListBox::outlineColourId, pink.withAlpha(0.24f));
    }
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float startAngle, float endAngle,
                          juce::Slider&) override {
        const float r = juce::jmin((float)width, (float)height) * 0.36f;
        const float cx = x + width * 0.5f, cy = y + height * 0.5f;
        const float angle = startAngle + sliderPos * (endAngle - startAngle);
        g.setColour(juce::Colour::fromRGB(228, 219, 228));
        g.fillEllipse(cx-r,cy-r,r*2,r*2);
        g.setColour(juce::Colour::fromRGB(179, 167, 184));
        g.drawEllipse(cx-r,cy-r,r*2,r*2,2.0f);
        juce::Path track;
        track.addCentredArc(cx,cy,r+8.0f,r+8.0f,0.0f,startAngle,endAngle,true);
        g.setColour(pink.withAlpha(0.24f)); g.strokePath(track,juce::PathStrokeType(3.0f));
        track.clear();track.addCentredArc(cx,cy,r+8.0f,r+8.0f,0.0f,startAngle,angle,true);
        g.setColour(pink);g.strokePath(track,juce::PathStrokeType(3.5f));
        juce::Path needle;
        needle.addRoundedRectangle(-1.7f,-r+8.0f,3.4f,r*0.50f,1.7f);
        g.setColour(juce::Colour::fromRGB(115, 49, 91));
        g.fillPath(needle,juce::AffineTransform::rotation(angle).translated(cx,cy));
    }
};
void styleLabel(juce::Label& label, float height, juce::Colour colour, bool bold = false) {
    label.setFont(juce::Font(juce::FontOptions(height, bold ? juce::Font::bold : juce::Font::plain)));
    label.setColour(juce::Label::textColourId, colour);
    label.setJustificationType(juce::Justification::centredLeft);
}
}

NeonDoseAudioProcessorEditor::NeonDoseAudioProcessorEditor(NeonDoseAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), styles(nd::makeStyles()),
      applyButton("BUILD VOCAL STYLE"), softButton("SOFTER"), wideButton("WIDER"), edgeButton("MORE EDGE") {
    nativeLaf = std::make_unique<NeonLook>();
    setLookAndFeel(nativeLaf.get());
    setSize(1040, 720);
    setResizable(false, false);

    styleLabel(heroTitle, 37.0f, pink, true);
    heroTitle.setText("NEONDOSE", juce::dontSendNotification);
    addAndMakeVisible(heroTitle);
    styleLabel(heroSubtitle, 14.0f, faint);
    heroSubtitle.setText("VOCAL STYLE LAB  /  NATIVE VST3  /  REAL-TIME DSP", juce::dontSendNotification);
    addAndMakeVisible(heroSubtitle);
    styleLabel(status, 15.0f, cream, true);
    status.setText("BEDROOM EMO RAP / CORE", juce::dontSendNotification);
    addAndMakeVisible(status);
    styleLabel(note, 12.0f, faint);
    note.setText("168 production recipes · not cloned artist voices · built for DAW monitoring", juce::dontSendNotification);
    note.setMinimumHorizontalScale(0.75f);
    addAndMakeVisible(note);
    styleLabel(sectionLibrary, 17.0f, pink, true);
    sectionLibrary.setText("STYLE LIBRARY", juce::dontSendNotification);
    addAndMakeVisible(sectionLibrary);
    styleLabel(sectionPrompt, 17.0f, pink, true);
    sectionPrompt.setText("CUSTOM STYLE PROMPT", juce::dontSendNotification);
    addAndMakeVisible(sectionPrompt);

    for (size_t i = 0; i < dials.size(); ++i) {
        auto& dial = dials[i];
        dial.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        dial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 94, 22);
        dial.setColour(juce::Slider::textBoxTextColourId, cream);
        dial.setColour(juce::Slider::textBoxBackgroundColourId, ink);
        dial.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        if (i == 7) dial.setTextValueSuffix(" dB");
        else dial.setTextValueSuffix(" %");
        addAndMakeVisible(dial);
        auto& label = dialNames[i];
        styleLabel(label, 12.0f, faint, true);
        label.setJustificationType(juce::Justification::centred);
        label.setText(labels[i], juce::dontSendNotification);
        addAndMakeVisible(label);
        attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.apvts, ids[i], dial);
    }

    categories.addItem("All styles", 1);
    const char* cats[] = {"Emo / Indie","Trap / Rap","Metal / Rock","Pop / R&B","Electronic","Lo-fi / Alt","Experimental"};
    for (int i = 0; i < 7; ++i) categories.addItem(cats[i], i+2);
    addAndMakeVisible(categories);
    categories.setSelectedId(1, juce::dontSendNotification);
    categories.onChange = [this] { filterPresets(); };

    search.setTextToShowWhenEmpty("Search 168 recipes...", faint.withAlpha(0.65f));
    search.setMultiLine(false);
    search.onTextChange = [this] { filterPresets(); };
    addAndMakeVisible(search);

    presetList.setModel(this);
    presetList.setRowHeight(31);
    presetList.setOutlineThickness(1);
    addAndMakeVisible(presetList);

    prompt.setMultiLine(true);
    prompt.setReturnKeyStartsNewLine(true);
    prompt.setTextToShowWhenEmpty("Describe the production style in full sentences: intimate sad emo rap with warm tape, very little distortion, a short room, no delay...", faint.withAlpha(0.65f));
    prompt.setText("Soft emotional bedroom emo-rap, warm tape, intimate vocal, less drive, no delay");
    addAndMakeVisible(prompt);

    applyButton.onClick = [this] { buildPrompt(); };
    softButton.onClick  = [this] { selectVariation(1); };
    wideButton.onClick  = [this] { selectVariation(2); };
    edgeButton.onClick  = [this] { selectVariation(3); };
    for (auto* b : { &applyButton, &softButton, &wideButton, &edgeButton }) addAndMakeVisible(*b);
    applyButton.setColour(juce::TextButton::buttonColourId, pink);
    applyButton.setColour(juce::TextButton::textColourOffId, ink);

    filterPresets();
    const auto savedPrompt = p.apvts.state.getProperty("userPrompt").toString();
    if (savedPrompt.isNotEmpty()) prompt.setText(savedPrompt);
    const auto savedStyle = p.apvts.state.getProperty("lastStyle").toString();
    if (savedStyle.isNotEmpty()) updateStatus(savedStyle);
}

NeonDoseAudioProcessorEditor::~NeonDoseAudioProcessorEditor() {
    presetList.setModel(nullptr);
    setLookAndFeel(nullptr);
}

void NeonDoseAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour::fromRGB(225, 218, 227));
    auto bezel = getLocalBounds().toFloat().reduced(9.0f);
    g.setColour(juce::Colour::fromRGB(249, 245, 248));
    g.fillRoundedRectangle(bezel, 20.0f);
    g.setColour(juce::Colour::fromRGB(167, 153, 171));
    g.drawRoundedRectangle(bezel, 20.0f, 2.0f);
    g.setColour(ink);
    g.fillRoundedRectangle(juce::Rectangle<float>(23, 22, 994, 220), 13.0f);
    g.setColour(pink.withAlpha(0.25f));
    g.drawRoundedRectangle(juce::Rectangle<float>(31, 30, 978, 204), 10.0f, 2.0f);
    for (int i = 0; i < 31; ++i) {
        g.setColour(pink.withAlpha(0.025f));
        g.drawHorizontalLine(40 + i * 6, 34.0f, 1006.0f);
    }
    g.setColour(juce::Colour::fromRGB(29, 20, 35));
    g.fillRoundedRectangle(juce::Rectangle<float>(24, 251, 992, 167), 11.0f);
    g.setColour(pink.withAlpha(0.22f));
    g.drawRoundedRectangle(juce::Rectangle<float>(24, 251, 992, 167), 11.0f, 1.0f);
    g.setColour(juce::Colour::fromRGB(30, 18, 35));
    g.fillRoundedRectangle(juce::Rectangle<float>(24, 427, 992, 272), 11.0f);
    g.setColour(pink.withAlpha(0.20f));
    g.drawRoundedRectangle(juce::Rectangle<float>(24, 427, 992, 272), 11.0f, 1.0f);
    g.setColour(pink.withAlpha(0.38f));
    g.drawVerticalLine(531, 445.0f, 680.0f);
    g.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
    g.setColour(faint);
    g.drawText("PROMPT-AWARE VOCAL FX   ·   NOT AN AI VOICE CLONER", 55, 211, 770, 20,
               juce::Justification::centredLeft, false);
    g.drawText("OUTPUT SAFETY  •  REDUCED DRIVE GAIN  •  DAW AUTOMATION", 550, 659, 448, 22,
               juce::Justification::centredLeft, false);
}

void NeonDoseAudioProcessorEditor::resized() {
    heroTitle.setBounds(54, 54, 610, 53);
    heroSubtitle.setBounds(56, 109, 840, 25);
    status.setBounds(56, 153, 910, 31);
    note.setBounds(56, 184, 900, 24);
    constexpr int left = 36, interval = 122;
    for (size_t i = 0; i < dials.size(); ++i) {
        int x = left + (int)i * interval;
        dialNames[i].setBounds(x, 265, 112, 22);
        dials[i].setBounds(x, 285, 112, 116);
    }
    sectionLibrary.setBounds(46, 443, 300, 25);
    sectionPrompt.setBounds(549, 443, 430, 25);
    categories.setBounds(45, 477, 188, 29);
    search.setBounds(243, 477, 269, 29);
    presetList.setBounds(45, 515, 466, 164);
    prompt.setBounds(550, 477, 445, 111);
    applyButton.setBounds(550, 597, 445, 32);
    softButton.setBounds(550, 636, 140, 27);
    wideButton.setBounds(702, 636, 140, 27);
    edgeButton.setBounds(855, 636, 140, 27);
}

int NeonDoseAudioProcessorEditor::getNumRows() { return (int)visibleStyles.size(); }

void NeonDoseAudioProcessorEditor::paintListBoxItem(int row, juce::Graphics& g,
                                                     int width, int height, bool selected) {
    if (row < 0 || row >= (int)visibleStyles.size()) return;
    const auto& s = styles[(size_t)visibleStyles[(size_t)row]];
    g.setColour(selected ? pink.withAlpha(0.27f) : (row % 2 == 0 ? ink : panel));
    g.fillRect(0,0,width,height);
    g.setColour(selected ? juce::Colours::white : cream);
    g.setFont(juce::Font(juce::FontOptions(13.0f, selected ? juce::Font::bold : juce::Font::plain)));
    g.drawFittedText(s.name, 10, 2, width-20, height-3, juce::Justification::centredLeft, 1);
}

void NeonDoseAudioProcessorEditor::selectedRowsChanged(int lastRowSelected) {
    if (lastRowSelected >= 0 && lastRowSelected < (int)visibleStyles.size())
        loadPreset(visibleStyles[(size_t)lastRowSelected]);
}

void NeonDoseAudioProcessorEditor::filterPresets() {
    visibleStyles.clear();
    const auto category = categories.getSelectedId() <= 1 ? juce::String() : categories.getText();
    const auto terms = juce::StringArray::fromTokens(search.getText().trim().toLowerCase(), " ,;", "\"");
    for (size_t i = 0; i < styles.size(); ++i) {
        const auto& s = styles[i];
        if (category.isNotEmpty() && s.category != category) continue;
        bool matches = true;
        for (const auto& term : terms)
            if (term.isNotEmpty() && !s.tags.containsIgnoreCase(term) && !s.name.containsIgnoreCase(term))
                matches = false;
        if (matches) visibleStyles.push_back((int)i);
    }
    presetList.updateContent();
    presetList.deselectAllRows();
    presetList.repaint();
}

void NeonDoseAudioProcessorEditor::updateStatus(const juce::String& message) {
    status.setText(message.toUpperCase(), juce::dontSendNotification);
}

void NeonDoseAudioProcessorEditor::loadPreset(int index) {
    if (index < 0 || index >= (int)styles.size()) return;
    const auto& s = styles[(size_t)index];
    processor.applyValues(s.values);
    processor.apvts.state.setProperty("lastStyle", s.name, nullptr);
    havePrompt = false;
    updateStatus(s.name);
    note.setText(s.description + "  ·  Production effect recipe, not a real voice model",
                 juce::dontSendNotification);
}

void NeonDoseAudioProcessorEditor::buildPrompt() {
    if (prompt.getText().trim().isEmpty()) {
        updateStatus("Enter a vocal-production description first");
        return;
    }
    const auto result = nd::interpretPrompt(prompt.getText(), styles);
    promptValues = result.values;
    havePrompt = true;
    processor.applyValues(promptValues);
    processor.apvts.state.setProperty("userPrompt", prompt.getText(), nullptr);
    processor.apvts.state.setProperty("lastStyle", "Custom prompt", nullptr);
    updateStatus("CUSTOM PROMPT / ACTIVE");
    note.setText(result.note, juce::dontSendNotification);
}

void NeonDoseAudioProcessorEditor::selectVariation(int kind) {
    if (!havePrompt) { updateStatus("Build a prompt before changing its variation"); return; }
    auto v = promptValues;
    if (kind == 1) {
        v[0] *= 0.55f; v[3] = juce::jmax(20.0f, v[3] - 12.0f);
        v[4] *= 0.55f; v[5] *= 0.55f;
    } else if (kind == 2) {
        v[2] = juce::jmin(90.0f, v[2] + 13.0f);
        v[4] = juce::jmin(86.0f, v[4] + 25.0f);
        v[5] = juce::jmin(86.0f, v[5] + 19.0f);
    } else if (kind == 3) {
        v[0] = juce::jmin(70.0f, v[0] + 17.0f);
        v[3] = juce::jmin(88.0f, v[3] + 8.0f);
        v[7] = juce::jmin(v[7], -5.0f);
    }
    processor.applyValues(v);
    updateStatus(kind == 1 ? "PROMPT / SOFTER" : kind == 2 ? "PROMPT / WIDER" : "PROMPT / MORE EDGE");
}

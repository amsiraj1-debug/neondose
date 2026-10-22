#pragma once
#include "StyleLibrary.h"
#include <regex>

namespace nd {
struct PromptResult {
    std::array<float, 8> values{};
    int coreIndex = 0;
    juce::String note;
};

inline PromptResult interpretPrompt(const juce::String& prompt, const std::vector<Style>& styles) {
    const juce::String txt = prompt.toLowerCase().trim();
    PromptResult result;
    if (styles.empty()) return result;
    const auto words = juce::StringArray::fromTokens(txt, " ,.!?;:/\\-()[]\n\t", "\"");
    const std::pair<const char*, int> references[] = {
        {"powfu",0},{"pow fu",0},{"kid laroi",1},{"laroi",1},
        {"drake",6},{"travis scott",7},{"neb",8},{"korusai",8},
        {"black sabbath",12},{"ozzy",12},{"iron maiden",13},{"bruce dickinson",13},
        {"current joy",4}
    };
    int best = 0;
    float maxScore = -1.0f;
    for (int i = 0; i < (int)styles.size(); i += 4) {
        const auto& s = styles[(size_t)i];
        float score = 0.0f;
        for (const auto& w : words) {
            if (w.length() < 3) continue;
            if (s.tags.containsWholeWordIgnoreCase(w)) score += 2.0f;
            else if (w.length() > 4 && s.tags.containsIgnoreCase(w)) score += 0.5f;
        }
        for (const auto& ref : references)
            if (txt.containsIgnoreCase(ref.first) && s.coreIndex == ref.second) score += 26.0f;
        if ((txt.contains("sad") || txt.contains("emo") || txt.contains("heartbreak")) && s.category == "Emo / Indie") score += 2.0f;
        if ((txt.contains("trap") || txt.contains("rap")) && s.category == "Trap / Rap") score += 2.0f;
        if ((txt.contains("metal") || txt.contains("heavy")) && s.category == "Metal / Rock") score += 2.0f;
        if (score > maxScore) { maxScore = score; best = i; }
    }
    if (maxScore < 2.0f) best = 0;
    result.coreIndex = best / 4;
    result.values = styles[(size_t)best].values;
    result.note = "Starting point: " + styles[(size_t)best].name;

    const auto has = [&](std::initializer_list<const char*> terms) {
        for (auto* term : terms) if (txt.containsIgnoreCase(term)) return true;
        return false;
    };
    const auto adjust = [&](size_t i, float amount) { result.values[i] += amount; };
    if (has({"intimate","close","bedroom","whisper","upfront"})) {
        adjust(0,-3); adjust(3,-6); adjust(4,-8); adjust(5,-7); result.note += " · intimate";
    }
    if (has({"sad","heartbreak","vulnerable","fragile","gentle","melancholy"})) {
        adjust(0,-5); adjust(3,-3); adjust(1,-4); adjust(4,5); result.note += " · tender";
    }
    if (has({"warm","analog","tape","cassette","dusty","vintage","lofi","lo-fi"})) {
        adjust(0,4); adjust(1,-9); adjust(2,-6); result.note += " · tape warmth";
    }
    if (has({"clear","clean","transparent","pristine"})) {
        adjust(0,-7); adjust(1,7); adjust(2,5); adjust(5,-3); result.note += " · clean";
    }
    if (has({"dark","muted","moody","nocturnal"})) { adjust(1,-11); adjust(2,-10); }
    if (has({"bright","airy","breathy","shimmer","gloss"})) { adjust(1,6); adjust(2,13); }
    if (has({"gritty","saturation","grit","distorted","fuzz","raspy"})) { adjust(0,13); adjust(1,-3); adjust(3,3); }
    if (has({"aggressive","growl","heavy","scream","powerful"})) { adjust(0,12); adjust(3,11); adjust(1,5); }
    if (has({"dreamy","spacious","ambient","distant","ethereal","cavernous","washed"})) {
        adjust(4,15); adjust(5,11); adjust(2,4); result.note += " · dreamy";
    }
    if (has({"reverb","hall","roomy","lush"})) { adjust(4,16); result.note += " · reverb"; }
    if (has({"echo","delay","slapback"})) { adjust(5,17); result.note += " · echoes"; }
    if (has({"punchy","tight","compressed","dense"})) { adjust(3,12); adjust(4,-6); adjust(5,-7); }

    // Negative directions override any positive descriptions. "Less" reduces the effect;
    // "no/without" disables the requested saturation or send entirely.
    const char* names[] = { "drive", "tone", "air", "compression", "reverb", "delay" };
    for (int i = 0; i < 6; ++i) {
        juce::StringArray aliases;
        aliases.add(names[i]);
        if (i == 0) { aliases.add("distortion"); aliases.add("saturation"); aliases.add("grit"); }
        if (i == 4) { aliases.add("space"); aliases.add("echo"); }
        if (i == 5) aliases.add("echo");
        for (const auto& alias : aliases) {
            if (txt.contains("no " + alias) || txt.contains("without " + alias)
                || txt.contains("remove " + alias) || txt.contains("avoid " + alias)) {
                if (i == 0 || i == 4 || i == 5) result.values[(size_t)i] = 0;
                else result.values[(size_t)i] -= 20;
            } else if (txt.contains("less " + alias) || txt.contains("reduce " + alias)
                       || txt.contains("lower " + alias)) {
                result.values[(size_t)i] -= 20;
            }
        }
    }
    // Explicit numeric overrides take precedence over descriptive modifiers.
    const std::string plain = txt.toStdString();
    const std::regex numberPattern(R"((drive|output|volume|gain)\s*(?:to|at|=)\s*([+-]?\d{1,3}))");
    std::smatch match;
    if (std::regex_search(plain, match, numberPattern)) {
        const bool output = match[1] != "drive";
        const float value = (float)std::stof(match[2]);
        result.values[output ? 7u : 0u] = value;
    }
    for (size_t i = 0; i < result.values.size(); ++i)
        result.values[i] = juce::jlimit(i == 7 ? -18.0f : 0.0f,
                                        i == 7 ? 6.0f : 100.0f,
                                        std::round(result.values[i]));
    if (result.values[0] > 44.0f) result.values[7] = juce::jmin(result.values[7], -5.0f);
    if (has({"autotune","auto-tune","clone", "exact voice", "voice conversion", "sound exactly"}))
        result.note += " · voice cloning/autotune not included";
    return result;
}
} // namespace nd

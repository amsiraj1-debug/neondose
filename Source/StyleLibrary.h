#pragma once
#include <juce_core/juce_core.h>
#include <array>
#include <vector>

namespace nd {
struct Style {
    juce::String name, category, description, tags;
    std::array<float, 8> values;
    int coreIndex = 0, variant = 0;
};

inline std::vector<Style> makeStyles() {
    struct Core { const char* name; const char* category; const char* description;
                  const char* tags; std::array<float, 8> values; };
    const std::vector<Core> cores = {
        { "Bedroom Emo Rap", "Emo / Indie", "Intimate tape-colored verses, shy melodic hooks", "powfu soft sad diary emo lo-fi bedroom", { 8.0f, 44.0f, 35.0f, 43.0f, 15.0f, 10.0f, 100.0f, -3.0f } },
        { "Heartbreak Pop Rap", "Emo / Indie", "Youthful close vocals with melodic pop gloss", "the kid laroi emo pop rap heartbreak melodic", { 12.0f, 62.0f, 55.0f, 56.0f, 26.0f, 19.0f, 100.0f, -4.0f } },
        { "Rainy Window", "Emo / Indie", "Muted late-night melancholy and damp-room ambience", "sad sleepy soft acoustic lo-fi", { 5.0f, 31.0f, 24.0f, 32.0f, 40.0f, 13.0f, 100.0f, -3.0f } },
        { "Acoustic Confession", "Emo / Indie", "Clear restrained singer-songwriter delivery", "acoustic indie intimate delicate confession", { 4.0f, 55.0f, 40.0f, 30.0f, 18.0f, 7.0f, 100.0f, -3.0f } },
        { "Indie Bedroom Tape", "Emo / Indie", "Dusty cassette warmth and inward-sounding vocal", "bedroom indie current joys dusty vintage", { 13.0f, 35.0f, 22.0f, 34.0f, 24.0f, 15.0f, 100.0f, -3.0f } },
        { "Fragile Falsetto", "Emo / Indie", "Soft, airy upper-register presence", "falsetto vulnerable emotional whisper", { 4.0f, 58.0f, 73.0f, 38.0f, 32.0f, 16.0f, 100.0f, -3.0f } },
        { "Nocturnal Melodic Rap", "Trap / Rap", "Warm low-mid body with reserved late-night echo", "drake melodic rap nocturnal moody", { 11.0f, 49.0f, 30.0f, 55.0f, 25.0f, 31.0f, 100.0f, -4.0f } },
        { "Psychedelic Trap", "Trap / Rap", "Smoky saturation and cavernous rhythmic repeats", "travis scott psychedelic trap dreamy atmospheric", { 26.0f, 41.0f, 47.0f, 53.0f, 40.0f, 48.0f, 100.0f, -5.0f } },
        { "Cloud Rap Drift", "Trap / Rap", "Hazy low-key vocal washed in delay", "neb korusai underground cloud rap dreamy", { 7.0f, 41.0f, 56.0f, 38.0f, 49.0f, 57.0f, 100.0f, -4.0f } },
        { "Drill Focus", "Trap / Rap", "Tight upfront low-mid vocal with limited ambience", "drill hard rap punchy focused upfront", { 22.0f, 50.0f, 33.0f, 71.0f, 8.0f, 9.0f, 100.0f, -5.0f } },
        { "Classic Hip-Hop", "Trap / Rap", "Punchy clean verses with short vintage ambience", "old school hip hop golden era", { 17.0f, 48.0f, 24.0f, 63.0f, 12.0f, 10.0f, 100.0f, -4.0f } },
        { "Melodic Trap Gloss", "Trap / Rap", "Airy compressed modern pop-trap vocal", "melodic trap gloss commercial clean", { 16.0f, 60.0f, 65.0f, 68.0f, 22.0f, 30.0f, 100.0f, -5.0f } },
        { "Vintage Doom", "Metal / Rock", "Dark vintage heavy-rock voice, restrained grime", "black sabbath vintage doom metal ozzy style", { 29.0f, 34.0f, 19.0f, 59.0f, 15.0f, 11.0f, 100.0f, -5.0f } },
        { "Soaring Heavy Metal", "Metal / Rock", "Upfront high-mid bite with clear sustained vocals", "iron maiden classic heavy metal soaring power", { 20.0f, 75.0f, 60.0f, 62.0f, 16.0f, 14.0f, 100.0f, -5.0f } },
        { "Modern Metalcore", "Metal / Rock", "Dense present rock vocal with controlled edge", "metalcore modern heavy angry", { 37.0f, 63.0f, 45.0f, 76.0f, 10.0f, 11.0f, 100.0f, -6.0f } },
        { "Grunge Tape", "Metal / Rock", "Weathered analog rock with a slightly rough body", "grunge alt rock vintage emotional", { 28.0f, 43.0f, 25.0f, 59.0f, 19.0f, 15.0f, 100.0f, -5.0f } },
        { "Power Ballad", "Metal / Rock", "Wide reverb behind a bright front vocal", "hard rock power ballad epic soaring", { 16.0f, 70.0f, 51.0f, 67.0f, 43.0f, 23.0f, 100.0f, -5.0f } },
        { "Post-Hardcore", "Metal / Rock", "Tense mid-forward rock with gritty edges", "post hardcore emo scream aggressive", { 33.0f, 60.0f, 39.0f, 73.0f, 18.0f, 12.0f, 100.0f, -6.0f } },
        { "Silky R&B", "Pop / R&B", "Close soft R&B shine with restrained dynamics", "soft smooth soul rnb silky", { 7.0f, 57.0f, 58.0f, 41.0f, 28.0f, 20.0f, 100.0f, -3.0f } },
        { "Dream Pop", "Pop / R&B", "Soft-focus spacious shimmer and glossy treble", "dream pop ethereal floating", { 6.0f, 57.0f, 73.0f, 42.0f, 61.0f, 37.0f, 100.0f, -4.0f } },
        { "Modern Pop Lead", "Pop / R&B", "Bright, radio-friendly vocal presence", "modern pop radio catchy studio", { 10.0f, 70.0f, 61.0f, 71.0f, 24.0f, 18.0f, 100.0f, -4.0f } },
        { "Warm Neo-Soul", "Pop / R&B", "Rounded harmonics with smooth subtle ambience", "neo soul warm organic jazzy", { 9.0f, 40.0f, 27.0f, 48.0f, 20.0f, 13.0f, 100.0f, -3.0f } },
        { "Dark Alt-Pop", "Pop / R&B", "Understated edge, dark presence and texture", "alternative pop dark mysterious", { 16.0f, 38.0f, 29.0f, 55.0f, 34.0f, 25.0f, 100.0f, -4.0f } },
        { "Hyperpop Edge", "Pop / R&B", "Bright synthetic brilliance with edgy saturation", "hyperpop shiny digital experimental", { 35.0f, 79.0f, 80.0f, 72.0f, 33.0f, 31.0f, 100.0f, -6.0f } },
        { "Vaporwave Haze", "Electronic", "Retro softened top end with diffuse tails", "vaporwave retro dreamy nostalgic", { 9.0f, 27.0f, 31.0f, 41.0f, 68.0f, 47.0f, 100.0f, -4.0f } },
        { "Synthwave Night", "Electronic", "Glossy neon ambience and steady presence", "synthwave electronic retro 80s", { 13.0f, 57.0f, 50.0f, 57.0f, 38.0f, 40.0f, 100.0f, -4.0f } },
        { "Ambient Electronica", "Electronic", "Weightless vocal tones and very long room tails", "ambient electronic ethereal space", { 4.0f, 47.0f, 69.0f, 35.0f, 83.0f, 30.0f, 100.0f, -4.0f } },
        { "UK Garage Vocal", "Electronic", "Rhythmic vocal brightness and compact repeats", "garage house uk electronic club", { 13.0f, 69.0f, 61.0f, 59.0f, 17.0f, 38.0f, 100.0f, -4.0f } },
        { "Industrial Pulse", "Electronic", "Metallic vocal color and raw tight saturation", "industrial electronic mechanical aggressive", { 42.0f, 52.0f, 36.0f, 71.0f, 24.0f, 22.0f, 100.0f, -6.0f } },
        { "Soft House Hook", "Electronic", "Polished dance-pop with lush room texture", "house melodic light dance", { 8.0f, 66.0f, 67.0f, 62.0f, 38.0f, 26.0f, 100.0f, -4.0f } },
        { "Dusty Cassette", "Lo-fi / Alt", "Worn warm tape character, low mids", "lofi cassette tape vintage dusty", { 17.0f, 26.0f, 20.0f, 41.0f, 16.0f, 8.0f, 100.0f, -5.0f } },
        { "Cloudy Underground", "Lo-fi / Alt", "Relaxed hazy bars with generous echoes", "neb korusai cloudy experimental rap underground", { 10.0f, 37.0f, 57.0f, 39.0f, 49.0f, 53.0f, 100.0f, -5.0f } },
        { "Shoegaze Distant", "Lo-fi / Alt", "Diffuse and submerged soft-rock vocals", "shoegaze washed out dreamy distant", { 13.0f, 40.0f, 44.0f, 47.0f, 78.0f, 40.0f, 100.0f, -5.0f } },
        { "Dark Bedroom Whisper", "Lo-fi / Alt", "Muted close vocal with barely-there saturation", "whisper soft close dark late-night", { 4.0f, 29.0f, 22.0f, 29.0f, 21.0f, 9.0f, 100.0f, -3.0f } },
        { "Lo-fi Jazz Booth", "Lo-fi / Alt", "Warmed low-volume croon, smoky short ambience", "jazz lo-fi mellow analog", { 8.0f, 36.0f, 19.0f, 36.0f, 25.0f, 9.0f, 100.0f, -3.0f } },
        { "Acoustic Dreamer", "Lo-fi / Alt", "Gentle intimate vocal with diffused high air", "acoustic dreamy breathy folk", { 3.0f, 52.0f, 64.0f, 32.0f, 48.0f, 15.0f, 100.0f, -3.0f } },
        { "Ghost Chorus", "Experimental", "Unnatural open room with dark spectral wash", "ghost haunted dark ambient experimental", { 11.0f, 27.0f, 61.0f, 49.0f, 89.0f, 65.0f, 100.0f, -6.0f } },
        { "Radio Transmission", "Experimental", "Band-limited vintage broadcast texture", "radio telephone filtered vintage voice", { 22.0f, 19.0f, 9.0f, 58.0f, 8.0f, 9.0f, 100.0f, -5.0f } },
        { "Digital Nebula", "Experimental", "Glassy airy saturation and sparkling repeats", "digital space futuristic ethereal", { 25.0f, 68.0f, 81.0f, 57.0f, 65.0f, 64.0f, 100.0f, -6.0f } },
        { "Underground Cyber", "Experimental", "Neon distorted underground vocal atmosphere", "cyber glitch experimental neb korusai", { 30.0f, 47.0f, 64.0f, 64.0f, 45.0f, 51.0f, 100.0f, -6.0f } },
        { "Dream Journal", "Experimental", "Slow-moving foggy vocal effect bed", "dream vapor ambient sleepy surreal", { 5.0f, 34.0f, 54.0f, 32.0f, 74.0f, 50.0f, 100.0f, -4.0f } },
        { "Intimate Minimalism", "Experimental", "Nearly dry close-up whisper with quiet dynamics", "minimal intimate clear transparent", { 0.0f, 53.0f, 45.0f, 28.0f, 2.0f, 0.0f, 100.0f, -3.0f } }
    };
    std::vector<Style> all;
    all.reserve(cores.size() * 4);
    const char* variations[] = { "Core", "Soft", "Wide", "Edge" };
    for (int i = 0; i < static_cast<int>(cores.size()); ++i) {
        for (int variant = 0; variant < 4; ++variant) {
            auto v = cores[(size_t) i].values;
            if (variant == 1) {
                v[0] *= 0.55f; v[3] = juce::jmax(20.0f, v[3] - 12.0f);
                v[4] *= 0.55f; v[5] *= 0.55f; v[7] = juce::jmin(v[7], -3.0f);
            } else if (variant == 2) {
                v[2] = juce::jmin(90.0f, v[2] + 13.0f);
                v[4] = juce::jmin(86.0f, v[4] + 25.0f);
                v[5] = juce::jmin(86.0f, v[5] + 19.0f);
                v[7] = juce::jmin(v[7], -4.0f);
            } else if (variant == 3) {
                v[0] = juce::jmin(70.0f, v[0] + 17.0f);
                v[3] = juce::jmin(88.0f, v[3] + 8.0f);
                v[4] = juce::jmax(5.0f, v[4] - 9.0f);
                v[7] = juce::jmin(v[7], -5.0f);
            }
            all.push_back({ juce::String(cores[(size_t) i].name) + " / " + variations[variant],
                            cores[(size_t) i].category, cores[(size_t) i].description,
                            (juce::String(cores[(size_t) i].name) + " " + cores[(size_t) i].category
                             + " " + cores[(size_t) i].description + " " + cores[(size_t) i].tags).toLowerCase(),
                            v, i, variant });
        }
    }
    return all;
}
} // namespace nd

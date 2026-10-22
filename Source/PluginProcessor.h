#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>

class NeonDoseAudioProcessor final : public juce::AudioProcessor {
public:
    NeonDoseAudioProcessor();
    ~NeonDoseAudioProcessor() override = default;
    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.5; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;
    void applyValues (const std::array<float, 8>&);

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    struct FilterState { float hpLp = 0.0f, toneLp = 0.0f, airLp = 0.0f; };
    std::array<FilterState, 2> filters{};
    juce::Reverb reverb;
    std::array<std::vector<float>, 2> delayLines;
    std::array<std::vector<float>, 2> reverbScratch;
    size_t delayPos = 0;
    float compEnvelope = 0.0f;
    double hz = 44100.0;
    int scratchCapacity = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NeonDoseAudioProcessor)
};

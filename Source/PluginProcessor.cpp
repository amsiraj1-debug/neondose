#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace {
const char* ids[] = { "drive", "tone", "air", "comp", "space", "delay", "mix", "output" };
float dbGain (float db) { return std::pow(10.0f, db / 20.0f); }
float coefficient (float cutoff, double sampleRate) {
    return 1.0f - std::exp(-juce::MathConstants<float>::twoPi * cutoff / (float) sampleRate);
}
}

NeonDoseAudioProcessor::NeonDoseAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "NEONDOSE_STATE", createParameters()) {}

juce::AudioProcessorValueTreeState::ParameterLayout NeonDoseAudioProcessor::createParameters() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    const float defaults[] = { 10, 47, 38, 39, 15, 12, 100, -3 };
    const char* names[] = { "Drive", "Tone", "Air", "Compression", "Space", "Delay", "Mix", "Output" };
    for (int i = 0; i < 8; ++i) {
        const float minimum = i == 7 ? -18.0f : 0.0f;
        const float maximum = i == 7 ? 6.0f : 100.0f;
        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(ids[i], 1), names[i],
            juce::NormalisableRange<float>(minimum, maximum, 1.0f), defaults[i]));
    }
    return layout;
}

void NeonDoseAudioProcessor::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) {
    hz = juce::jmax(8000.0, sampleRate);
    scratchCapacity = juce::jmax(256, maximumExpectedSamplesPerBlock);
    const auto delaySize = static_cast<size_t>(std::ceil(hz * 2.0)) + 8;
    for (auto& line : delayLines) line.assign(delaySize, 0.0f);
    for (auto& channel : reverbScratch) channel.assign((size_t) scratchCapacity, 0.0f);
    for (auto& filter : filters) filter = {};
    delayPos = 0;
    compEnvelope = 0;
    reverb.setSampleRate(hz);
    reverb.reset();
}

void NeonDoseAudioProcessor::releaseResources() {
    reverb.reset();
}

bool NeonDoseAudioProcessor::isBusesLayoutSupported(const BusesLayout& layout) const {
    const auto& in = layout.getMainInputChannelSet();
    return (in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo())
        && layout.getMainOutputChannelSet() == in;
}

void NeonDoseAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin(buffer.getNumChannels(), 2);
    const int frames = buffer.getNumSamples();
    if (channels == 0 || frames == 0 || delayLines[0].empty()) return;
    float p[8];
    for (int i = 0; i < 8; ++i) p[i] = apvts.getRawParameterValue(ids[i])->load();

    const float drive = p[0] / 100.0f;
    const float toneHz = 1250.0f + std::pow(p[1] / 100.0f, 1.55f) * 15750.0f;
    const float highpassA = coefficient(65.0f, hz);
    const float lowpassA = coefficient(juce::jmin(toneHz, (float)hz * 0.45f), hz);
    const float airA = coefficient(juce::jmin(5200.0f, (float) hz * 0.4f), hz);
    const float airAmount = dbGain((p[2] - 50.0f) * 0.17f) - 1.0f;
    const float threshold = -11.0f - p[3] * 0.22f;
    const float ratio = 1.2f + p[3] * 0.062f;
    const float attack = std::exp(-1.0f / ((float) hz * 0.010f));
    const float release = std::exp(-1.0f / ((float) hz * 0.155f));
    const float angle = p[6] * (juce::MathConstants<float>::halfPi / 100.0f);
    const float dryGain = std::cos(angle), wetGain = std::sin(angle);
    const float delayWet = std::pow(p[5] / 100.0f, 1.25f) * 0.24f * wetGain;
    const float reverbWet = std::pow(p[4] / 100.0f, 1.3f) * 0.32f * wetGain;
    const float feedback = 0.10f + p[5] * 0.0032f;
    const auto tap = (size_t) juce::jlimit(1, (int)delayLines[0].size() - 1,
                                         (int)std::round((0.10 + p[5] * 0.0053) * hz));
    const float satBlend = drive * 0.8f;
    const float satIntensity = 1.0f + drive * 5.5f;
    const float satNorm = std::pow(satIntensity, 0.8f);
    const float master = dbGain(p[7]) * 0.48f;

    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = juce::jlimit(0.0f, 0.95f, 0.25f + p[4] * 0.0065f);
    reverbParams.damping = 0.45f;
    reverbParams.wetLevel = 1.0f;
    reverbParams.dryLevel = 0.0f;
    reverbParams.width = 0.8f;
    reverb.setParameters(reverbParams);

    int done = 0;
    while (done < frames) {
        const int count = juce::jmin(frames - done, scratchCapacity);
        float* left = buffer.getWritePointer(0, done);
        float* right = channels == 2 ? buffer.getWritePointer(1, done) : nullptr;
        for (int n = 0; n < count; ++n) {
            const float raw[2] = { left[n], right ? right[n] : left[n] };
            float shaped[2]{};
            float envInput = 0.0f;
            for (int ch = 0; ch < 2; ++ch) {
                auto& f = filters[(size_t) ch];
                f.hpLp += highpassA * (raw[ch] - f.hpLp);
                const float hp = raw[ch] - f.hpLp;
                f.toneLp += lowpassA * (hp - f.toneLp);
                f.airLp += airA * (f.toneLp - f.airLp);
                shaped[ch] = f.toneLp + airAmount * (f.toneLp - f.airLp);
                envInput = juce::jmax(envInput, std::abs(shaped[ch]));
            }
            const float coeff = envInput > compEnvelope ? attack : release;
            compEnvelope = coeff * compEnvelope + (1.0f - coeff) * envInput;
            const float level = juce::Decibels::gainToDecibels(juce::jmax(1.0e-8f, compEnvelope));
            const float reductionDb = level > threshold ? (threshold + (level - threshold) / ratio - level) : 0.0f;
            const float compGain = dbGain(reductionDb);
            const size_t read = (delayPos + delayLines[0].size() - tap) % delayLines[0].size();
            for (int ch = 0; ch < 2; ++ch) {
                const float clean = shaped[ch] * compGain;
                const float sat = std::tanh(clean * satIntensity) / satNorm;
                const float stage = clean * (1.0f - satBlend) + sat * satBlend;
                const float previous = delayLines[(size_t) ch][read];
                delayLines[(size_t) ch][delayPos] = stage + previous * feedback;
                reverbScratch[(size_t) ch][(size_t)n] = stage;
                const float bus = raw[ch] * dryGain + stage * wetGain + previous * delayWet;
                if (ch == 0) left[n] = bus;
                else if (right) right[n] = bus;
            }
            delayPos = (delayPos + 1) % delayLines[0].size();
        }
        reverb.processStereo(reverbScratch[0].data(), reverbScratch[1].data(), count);
        for (int n = 0; n < count; ++n) {
            const float l = (left[n] + reverbWet * reverbScratch[0][(size_t)n]) * master;
            // Soft safety limiting at extreme input levels; no automatic make-up gain.
            left[n] = juce::jlimit(-0.98f, 0.98f, std::tanh(l * 1.2f) / 1.2f);
            if (right) {
                const float r = (right[n] + reverbWet * reverbScratch[1][(size_t)n]) * master;
                right[n] = juce::jlimit(-0.98f, 0.98f, std::tanh(r * 1.2f) / 1.2f);
            }
        }
        done += count;
    }
}

void NeonDoseAudioProcessor::applyValues(const std::array<float, 8>& values) {
    for (int i = 0; i < 8; ++i)
        if (auto* param = apvts.getParameter(ids[i]))
            param->setValueNotifyingHost(param->convertTo0to1(values[(size_t)i]));
}

void NeonDoseAudioProcessor::getStateInformation(juce::MemoryBlock& destination) {
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary(*xml, destination);
}

void NeonDoseAudioProcessor::setStateInformation(const void* data, int size) {
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessorEditor* NeonDoseAudioProcessor::createEditor() {
    return new NeonDoseAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new NeonDoseAudioProcessor();
}

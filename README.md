# NEONDOSE — Windows VST3 GitHub Actions project

Native JUCE/C++ **vocal effects** plugin based on the NEONDOSE 2 browser prototype.

## Included features

- **Actual VST3 audio processing**: DAW audio in -> HPF / tone LPF / air shelf -> dynamics compressor -> low-output-gain soft saturation -> filtered/timed delay + stereo reverb -> safe output.
- **168 vocal-production presets**: original 42 style cores in 4 variations (Core / Soft / Wide / Edge). They are *effects recipes*, not artist voice models.
- **Custom descriptive prompt box** in the VST3 editor: simple local rule-based interpretation of genre/texture/mood language and directions such as "no delay", "less drive", and "output to -6".
- 8 automation-ready parameters: Drive, Tone, Air, Comp, Space, Delay, Mix, Output.
- Native themed interface, category filtering, style search, prompt variations, project preset state.
- **Windows x64 GitHub Actions workflow** produces a downloadable .vst3 archive.
- `Website/` carries the prior browser application as a reference. It is not required by the plugin build.

> IMPORTANT: This is **not** a neural singer model, artist voice clone, text-to-audio generator, realtime AI voice changer, formant conversion or an autotune processor. Prompts change production effects; they do not change the singer's identity. No unlicensed artists' voice models are included.

## Build on GitHub (no Windows compiler needed on your PC)

1. Create a new empty **GitHub repository** (or choose an existing one).
2. Extract `NEONDOSE-VST3-GitHub-Source.zip` and upload **the contents of the `NEONDOSE-VST3-GitHub` folder**, preserving `CMakeLists.txt`, `Source/`, and `.github/workflows/build-vst3.yml` at repository root. Do not upload only the workflow: the source code is essential.
3. Go to **Actions -> Build NEONDOSE Windows VST3 -> Run workflow -> Run workflow**. Pushing the source to `main` or `master` also triggers the workflow.
4. Open the completed Actions run and download the artifact **NEONDOSE-Windows-x64-VST3**.
5. Extract that artifact ZIP and then extract the included `NEONDOSE-Windows-x64-VST3.zip` if GitHub supplied a surrounding artifact archive. Copy the **NEONDOSE.vst3 folder** into:

   `C:\Program Files\Common Files\VST3\`

6. In FL Studio / another DAW, rescan VST3 plugins, place NEONDOSE on a vocal mixer insert, and choose a preset or type a prompt.

The Actions job uses the `windows-2022` runner with Visual Studio 2022 and pins JUCE to **8.0.16-security**. GitHub downloads JUCE automatically during CMake configure. The runner needs GitHub network access. There are no secret tokens or activation steps.

## The smaller workflow-only ZIP

`NEONDOSE-GitHub-Actions-Only.zip` contains `.github/workflows/build-vst3.yml` and a workflow README. Use it **only if the matching project source is already in your repository**. A YAML file alone cannot compile a VST3.

## Build locally on Windows (optional)

Prerequisites: Visual Studio 2022 with "Desktop development with C++", CMake, Git.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target NeonDose_VST3 --parallel 2
```

The bundle normally appears under `build/NeonDose_artefacts/Release/VST3/`.

## Testing / audio notes

- Monitor real-time audio from the DAW. The plugin does **not** use a separate web server or external voice engine.
- Drive is mixed in parallel with clean signal, normalized, and then attenuated by the master. Start with low Drive and Output -3 dB.
- The processor uses preallocated delay/reverb scratch buffers and no file operations in `processBlock`.
- Reverb/delay tails can last after transport stops. The plugin reports a 2.5-second tail.
- Sessions remember host-automated parameters and the last prompt text; the prompt itself is interpreted on pressing **BUILD VOCAL STYLE**.
- For professional release, test in FL Studio, Reaper, and a VST3 validator at several sample rates and buffer sizes. Code has **not** been compiled or host-tested in this environment; the GitHub Actions run will reveal build/runtime issues if any.

## Licensing

JUCE is subject to its own commercial/GPL licensing options. Review the current JUCE license before distributing proprietary commercial builds. The NEONDOSE sample code is provided for this user's project; it contains no artist voice model assets.

# 🫧 Schlomo's Bath

**Vocal Life Engine™** - Making your vocals sound like actual humans in actual bathtubs

---

## What Is This?

Schlomo's Bath is a VST3 plugin that adds **human imperfection and bathtub ambience** to vocals. It's not a reverb, not a harmonizer, not a doubler. It's a **Vocal Life Engine** that simulates:

- **Human vocal randomness** (pitch drift, timing wobble, breath noise)
- **Bathtub environment chaos** (porcelain reflections, steam, water sounds)
- **Character modes** (rubber duck, soap bar glitch, bathroom fan)

Think of it as "making perfect digital vocals sound like they were recorded by an imperfect human in a real bathroom."

---

## Features

### 🎤 Human Vocal Randomizers
- **Pitch Drift Brain** - Natural pitch imperfections (not vibrato)
- **Formant Whispers** - Subtle vocal tract shape changes
- **Breath & Noise Engine** - Context-aware breath and air sounds
- **Timing Wobble** - Micro-timing variations for human feel
- **Volume Personality** - Realistic fader movements

### 💦 Bathtub Environment
- **Porcelain Reflections** - Chaotic early reflections (not static reverb)
- **Steam Modulator** - Humidity-based sonic warping
- **Faucet Flutter** *(coming soon)*
- **Drain Resonator** *(coming soon)*

### 🦆 Character Modes
- **Rubber Duck FM** - Formant-following FM quack effects
- **Soap Bar Glitch** - Slippery pitch drops and grain smears
- **Bathroom Fan Chorus** *(coming soon)*
- **Mildewy Wall Filter** *(coming soon)*

---

## Quick Start

### Building the Plugin

**One-Click Setup** (Windows):
```batch
quick_setup.bat
```

This will:
1. Download JUCE (if needed)
2. Build the VST3 plugin
3. Install it to your VST3 folder

**Manual Build:**
```batch
build.bat
```

**Install:**
```batch
install.bat
```

### Requirements
- Windows 10/11
- Visual Studio 2022 (Community Edition works)
- CMake 3.15+
- Git (for downloading JUCE)

---

## Current Status

**Version 0.1.0** - Framework Complete

✅ **Working:**
- Full modular architecture
- 9 vocal processing modules (framework)
- VST3 build system
- Basic GUI with module toggles
- Master wet/dry mix

🔨 **In Progress:**
- Individual DSP algorithm implementations
- Advanced parameter controls
- Preset system

See [FEATURE_ROADMAP.md](FEATURE_ROADMAP.md) for detailed implementation checklist.

---

## Project Structure

```
Schlomo-s-Bath/
├── Source/
│   ├── PluginProcessor.h/cpp    # Main VST3 processor
│   ├── PluginEditor.h/cpp       # GUI
│   └── VocalProcessor.h/cpp     # All DSP modules
├── build/                       # Build output (generated)
├── CMakeLists.txt              # Build configuration
├── build.bat                   # Windows build script
├── install.bat                 # Installation script
├── quick_setup.bat             # One-click setup
├── README.md                   # This file
└── FEATURE_ROADMAP.md          # Complete feature list
```

---

## How to Use

1. **Load the plugin** on a vocal track in your DAW
2. **Enable modules** you want (click to toggle on/off)
3. **Adjust Master Mix** knob to blend wet/dry
4. **Tweak individual modules** *(advanced controls coming soon)*

### Tips
- Start with **Breath & Noise Engine** + **Volume Personality** for subtle humanization
- Add **Porcelain Reflections** for bathroom ambience
- Go wild with **Rubber Duck FM** for creative effects
- Layer multiple modules for maximum chaos

---

## Philosophy

> Real humans don't sing perfectly in tune with perfect timing in a perfectly dead room.
> They wobble. They breathe. They search for the note.
> They sound like they're in a **room** (specifically, a bathtub).

This plugin makes digital vocals sound like they were:
- **Recorded by a human** (with all the imperfections)
- **Sung by a human** (with natural variations)
- **In a human space** (a humid, resonant bathroom)

---

## Roadmap

### Phase 1: Core DSP ✅
- [x] Modular architecture
- [x] Basic implementations for key modules
- [ ] Complete all 9 module DSP algorithms

### Phase 2: Missing Modules
- [ ] Faucet Flutter
- [ ] Drain Resonator
- [ ] Bathroom Fan Chorus
- [ ] Mildewy Wall Filter

### Phase 3: Intelligence
- [ ] Vowel/Consonant detection
- [ ] Behavior presets
- [ ] Voice Identity Builder
- [ ] Auto wet/dry automation

### Phase 4: Polish
- [ ] Advanced GUI with parameter controls
- [ ] Preset management
- [ ] CPU optimization
- [ ] Documentation & user manual

---

## Technical Details

- **Framework:** JUCE 7.x
- **Format:** VST3
- **Language:** C++17
- **DSP:** Custom algorithms + JUCE DSP modules
- **Sample Rates:** 44.1kHz - 192kHz supported
- **Latency:** Minimal (depends on enabled modules)

---

## Credits

**Created by:** Woody & Claude
**Inspired by:** The absurd creativity of bathtub acoustics
**Powered by:** JUCE Framework

---

## License

[Add your license here]

---

## Support

Having issues? Check:
1. [FEATURE_ROADMAP.md](FEATURE_ROADMAP.md) - Full implementation status
2. Build logs in `build/` folder
3. Make sure Visual Studio 2022 & CMake are installed

---

**🫧 Get in the bath. Make some weird vocals. 🦆**

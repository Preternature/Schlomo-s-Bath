# 🫧 SCHLOMO'S BATH - Feature Roadmap & Implementation Checklist

**Vocal Life Engine™ - Making vocals sound like actual humans in actual bathtubs**

---

## 🎯 PROJECT VISION
This is not a reverb. Not a modulator. Not a harmonizer. Not a doubler.
**This is a Vocal Life Engine™.**

---

## 📋 MODULE CATEGORY 1 — Human Vocal Randomizers
*These attack the human voice itself, not the environment.*

### 1. ✅ Pitch Drift Brain™
**Status:** Framework Complete | DSP: TODO

**Behavior-driven pitch variation**
- [ ] Vowel-based random glides
- [ ] Transient-related pitch overshoots
- [ ] Pitch "searching" (slow gliss into correct anchoring)
- [ ] Drunk mode (semi-quantized chaos around target pitch)
- [ ] Anxiety mode (higher, tremulous, unstable micro jumps)
- [ ] Not vibrato — actual micro-imperfections

**Implementation Notes:**
- Use pitch detection to find current note
- Apply random glide curves during vowel sustains
- Add micro-detuning based on behavior mode
- Implement pitch correction with intentional "searching" artifacts

---

### 2. ✅ Formant Whispers
**Status:** Framework Complete | DSP: TODO

**Random, low-level changes in vocal tract shape**
- [ ] Subtle nasalization
- [ ] Low/high throat resonance shifts
- [ ] Head turn formants (tilts brightness left/right)
- [ ] "Mouth shape wobble"
- [ ] Occasional formant pops on loud syllables

**Implementation Notes:**
- Build formant filter bank (vowel positions: /a/, /e/, /i/, /o/, /u/)
- Random subtle shifts between formant positions
- Add nasal resonance filter (~500-1000Hz emphasis)
- Brightness tilt based on random "head position"

---

### 3. ✅ Breath & Noise Engine
**Status:** Framework Complete | DSP: Basic Implementation

**Adds and modulates breath in context**
- [x] Breath loudness tied to syllable onset (basic envelope follower)
- [ ] Random "air whooshes" when dynamic peaks hit
- [ ] Noisy consonant enhancement
- [ ] "Blown-out" mic proximity puff simulation
- [ ] Huff mode: sharp noise bursts on stressed notes

**Implementation Notes:**
- Already has basic breath noise generation
- Add transient detector for "whoosh" triggers
- Enhance sibilance/consonant regions with noise
- Proximity effect: low-freq boost + breath increase

---

### 4. ✅ Timing Wobble
**Status:** Framework Complete | DSP: TODO

**Micro-timing deviations**
- [ ] Laggy phrasing
- [ ] Wandering timing envelope
- [ ] "Late on purpose" algorithm
- [ ] Random push/pull between vowels
- [ ] Optional crossfade toward swing feel

**Implementation Notes:**
- Variable delay line with random modulation
- Detect transients and apply micro-delays
- Swing quantization: every other beat slightly delayed
- Crossfade between dry and time-shifted signal

---

### 5. ✅ Volume Personality
**Status:** Framework Complete | DSP: Basic Implementation

**Fader movement as a human would move it**
- [x] Random micro-dips (nervous singer) - basic implementation
- [ ] Sudden brightness swells
- [ ] Plosive-triggered volume jumps
- [ ] Wavering loudness on held notes
- [ ] TikTok-compression-wobble mode

**Implementation Notes:**
- Already has basic gain wobble
- Add EQ automation for "brightness swells"
- Detect plosives (sudden low-freq energy) → gain jump
- Personality presets with different wobble characteristics

---

## 💦 MODULE CATEGORY 2 — Environmental / Bathtub Randomizers
*These simulate the chaos of being in a humid, resonant bathroom.*

### 6. ✅ Porcelain Reflections Engine
**Status:** Framework Complete | DSP: TODO

**Not reverb: tiny, chaotic echo spikes**
- [ ] Tile scatter (micro early reflections)
- [ ] Random delays from sink, mirror, shower door
- [ ] Reflection brightness changes with volume
- [ ] Edge-of-tub slap resonance
- [ ] Footstep / re-aim-induced phase flips

**Implementation Notes:**
- Multiple short delay lines (5-50ms)
- Random modulation of delay times (simulate moving head)
- Amplitude-dependent filtering on reflections
- Metallic resonance at ~800-2kHz

---

### 7. ✅ Steam Modulator
**Status:** Framework Complete | DSP: TODO

**Humidity causing sonic warping**
- [ ] Dampened transients when steam rises
- [ ] Gradual formant blur
- [ ] High-frequency melting at loud peaks
- [ ] Fog mode (airy floating HF hiss)
- [ ] "Temperature up / temperature down" drift

**Implementation Notes:**
- Low-pass filter with random automation
- Reduce transient peaks in "steam" mode
- Add diffuse white noise layer for fog
- Temperature parameter controls overall dampening

---

### 8. ⬜ Faucet Flutter
**Status:** Not Yet Implemented

**Water pressure → modulation**
- [ ] Fast random tremolo
- [ ] Flutter driven by broadband noisy bursts
- [ ] Metallic cold-water resonance
- [ ] Drip-triggered pitch flicks
- [ ] Hydro-LFO (fluid-based random waveshaper)

**Implementation Notes:**
- Random AM/tremolo with irregular rate
- Metallic resonator (comb filter + resonance)
- Random pitch glitches triggered by noise bursts
- LFO with drunk/wavy shape

---

### 9. ⬜ Drain Resonator
**Status:** Not Yet Implemented

**Filtered tonal whines beneath the voice**
- [ ] Optional low hum at 50–200 Hz
- [ ] Random resonant peaks when voice gets loud
- [ ] Metallic "pipe flutter"
- [ ] Hair-clog mode (gross but real lol)

**Implementation Notes:**
- Resonant bandpass filter at low frequencies
- Amplitude-triggered resonance
- Random formant peaks in 200-800Hz range
- "Clog" mode: muffled, unstable resonance

---

## 🩳 MODULE CATEGORY 3 — Funny / Chaotic / Character Modes
*These are WILD but usable in production.*

### 10. ✅ Rubber Duck FM
**Status:** Framework Complete | DSP: Basic Implementation

**Formant-following FM "quack" at behavior-based triggers**
- [x] Basic FM modulation
- [ ] Trigger on transients
- [ ] Wet quack mode
- [ ] Angry duck
- [ ] Slow wobble
- [ ] Cartoon slider

**Implementation Notes:**
- Already has simple FM wobble
- Add formant tracking → FM frequency
- Trigger "quacks" on loud syllables
- Cartoon mode: exaggerated FM ratios

---

### 11. ✅ Soap Bar Glitch
**Status:** Framework Complete | DSP: TODO

**Pitch + grain modulation reactions**
- [ ] Slippery pitch falls
- [ ] Grain smears during consonants
- [ ] Sudden slips (random downward pitch drop)
- [ ] Soapy blur only on vowels
- [ ] "Drop the soap" hard reset glitch

**Implementation Notes:**
- Pitch shifter with random downward glides
- Granular synthesis during consonant regions
- Random "slip" events → sudden pitch drop
- Blur mode: smear grains across vowels

---

### 12. ⬜ Bathroom Fan Chorus
**Status:** Not Yet Implemented

**Not a chorus — a vibrating, slightly broken fan blender**
- [ ] Noisy flutter
- [ ] Unstable pitch modulation
- [ ] HF shimmer tied to "fan RPM"
- [ ] Optional broken fan mode (sputters)

**Implementation Notes:**
- Chorus with irregular LFO (not sine wave)
- Add noise modulation to delay time
- High-freq shimmer (ring mod or subtle distortion)
- "Broken" mode: random dropouts

---

### 13. ⬜ Mildewy Wall Filter
**Status:** Not Yet Implemented

**Static + organic filtering**
- [ ] Slow HF smothering
- [ ] Random mid dampening
- [ ] Brownish, damp resonance
- [ ] Moldburst: sudden mid-spike

**Implementation Notes:**
- Slow-moving low-pass filter
- Random cuts in 500-2kHz range
- Resonant filter with "decay" character
- Random mid-boost bursts

---

## 📢 MODULE CATEGORY 4 — Macro Behavior Engine
*This ties all randomness together so the plugin feels "alive" instead of chaotic.*

### 14. ⬜ Behavior Curves
**Status:** Not Yet Implemented

**Global bounds for how random the system is**

**Preset Modes:**
- [ ] Shy (subtle, minimal randomization)
- [ ] Sloshed (chaotic drift everywhere)
- [ ] Nervous (fast, light modulations)
- [ ] Confident (controlled but still human)
- [ ] Ghost in Shower (weird phase / HF shifts)
- [ ] ASMR Gremlin (quiet, up-close, noisy)

**Implementation Notes:**
- Master randomness intensity control
- Each preset adjusts all module parameters
- Morphable between presets

---

### 15. ⬜ Vowel/Consonant IQ
**Status:** Not Yet Implemented

**Modules act differently depending on:**
- [ ] Vowel type (open, closed, rounded)
- [ ] Plosive strength
- [ ] Sibilant energy
- [ ] Voiced/unvoiced detection
- [ ] Onset vs sustained note

**Implementation Notes:**
- Spectral analysis to detect vowel formants
- Transient detection for plosives
- High-freq energy for sibilance
- Route different effects based on detected type

---

### 16. ⬜ Wet-Dry Mind
**Status:** Not Yet Implemented

**Wetness changes over time, based on:**
- [ ] Phrase length
- [ ] Amplitude
- [ ] Density

**Implementation Notes:**
- Not a static slider — behavior-based automation
- Louder = more wet (or less, depending on mode)
- Long sustained notes → gradual wetness increase

---

### 17. ⬜ Voice Identity Builder
**Status:** Not Yet Implemented

**Auto-randomizes:**
- [ ] Formants
- [ ] Timing
- [ ] Vibrato
- [ ] Brightness
- [ ] Attack style
- [ ] ...and saves it as "This Voice"

**Implementation Notes:**
- Preset generator with random parameters
- Lock in a "character" for consistent doubling
- Multiple voice slots

---

## 🔧 CURRENT BUILD STATUS

### ✅ Completed
- [x] CMake build system
- [x] JUCE VST3 project setup
- [x] Modular architecture (VocalModule base class)
- [x] All module class definitions
- [x] Basic plugin processor framework
- [x] GUI framework with module toggles
- [x] Master wet/dry mix control
- [x] Basic breath noise engine (envelope follower)
- [x] Basic volume personality (gain wobble)
- [x] Basic rubber duck FM

### 🔨 In Progress
- [ ] Individual module DSP implementations
- [ ] Advanced GUI controls (sliders for module parameters)
- [ ] Preset system
- [ ] Parameter automation

### 📝 TODO (Priority Order)
1. **Finish DSP for existing modules** (Phase 1)
   - Pitch Drift Brain
   - Formant Whispers
   - Timing Wobble
   - Porcelain Reflections
   - Steam Modulator
   - Soap Bar Glitch

2. **Implement missing modules** (Phase 2)
   - Faucet Flutter
   - Drain Resonator
   - Bathroom Fan Chorus
   - Mildewy Wall Filter

3. **Macro Behavior System** (Phase 3)
   - Behavior Curves (presets)
   - Vowel/Consonant detection
   - Wet-Dry automation
   - Voice Identity Builder

4. **Polish** (Phase 4)
   - Advanced GUI with parameter controls
   - Preset management
   - CPU optimization
   - User manual

---

## 🎨 GUI LAYOUT (Current)

```
┌──────────────────────────────────────────────────────┐
│               SCHLOMO'S BATH                         │
│               Vocal Life Engine                      │
├──────────────────────────────────────────────────────┤
│                                                      │
│  HUMAN VOCAL     ENVIRONMENT      CHARACTER          │
│  ┌─────────────┐ ┌──────────────┐ ┌──────────────┐ │
│  │Pitch Drift  │ │Porcelain Ref│ │Rubber Duck  │ │
│  │Formant Whis │ │Steam Mod    │ │Soap Bar Glit│ │
│  │Breath Noise │ └──────────────┘ └──────────────┘ │
│  │Timing Wobble│                                    │
│  │Volume Pers  │                                    │
│  └─────────────┘                                    │
│                                                      │
│              ┌─────────────┐                        │
│              │ Master Mix  │                        │
│              │    (Knob)   │                        │
│              └─────────────┘                        │
│                                                      │
│  ╭─────────────────────────────────────────────╮    │
│  │        🛁   Bathtub Graphic  🫧            │    │
│  │              (Decorative)                   │    │
│  ╰─────────────────────────────────────────────╯    │
└──────────────────────────────────────────────────────┘
```

---

## 🚀 QUICK START (Build Instructions)

1. **One-click setup:**
   ```
   quick_setup.bat
   ```

2. **Manual build:**
   ```
   build.bat
   ```

3. **Install to VST3 folder:**
   ```
   install.bat
   ```

---

## 📚 REFERENCES & INSPIRATION

- **pYIN Algorithm** - For pitch detection/formant analysis
- **Granular Synthesis** - For soap bar glitch
- **Physical Modeling** - For bathtub reflections
- **Spectral Processing** - For formant shifting
- **Chaotic Modulation** - For all the weird stuff

---

## 🎯 PHILOSOPHY

> "Real humans don't sing perfectly in tune with perfect timing in a perfectly dead room. They wobble, they breathe, they search for the note, and they sound like they're in a ROOM. This plugin makes digital vocals sound like they were recorded by a human, sung by a human, in a human space (specifically, a bathtub)."

**— Schlomo, probably**

---

*Last Updated: 2025-11-19*
*Version: 0.1.0 (Framework Complete)*

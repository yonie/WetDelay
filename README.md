# WET Delay VST3 Plugin

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![VST3](https://img.shields.io/badge/VST3-Compatible-blue)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey)
![Version](https://img.shields.io/badge/version-1.0.0-orange)

A professional stereo delay VST3 plugin with authentic 80s rack-style digital delay character.

![WetDelay Plugin Screenshot](screenshot.png)

## Features

- **100% Wet Delay**: Pure delayed signal output with no dry signal mix
- **6 Delay Times**: Switchable delay times (20ms, 40ms, 80ms, 120ms, 220ms, 400ms)
- **Stereo Processing**: Independent left and right channel delay processing
- **Visual Metering**: Real-time peak level meters for input and output
- **VST3 Automation**: Full parameter automation support in DAWs

### 80s Rack-Style Character

- **24 kHz Internal Sample Rate**: Authentic vintage digital delay processing with band-limited frequency response
- **12-bit Quantization**: Classic gritty digital character with 4096 discrete levels
- **TPDF Dither**: Smooth quantization with triangular probability density function dither (0.5 LSB)
- **-80 dBFS Noise Floor**: Realistic analog electronics and ADC/DAC noise simulation
- **Vintage Filtering**: 80 Hz high-pass and 9 kHz low-pass (6 dB/oct) for warm character
- **Channel Crosstalk**: Authentic -40 dB (1%) L/R channel bleed simulating analog circuitry

## System Requirements

- **Operating System**: Windows 10/11 (64-bit)
- **Build Tools**: 
  - Visual Studio 2022 Build Tools or Community Edition
  - CMake 3.15 or higher
  - Git

## Quick Start

### Building the Plugin

1. **Clone and build in one step:**
```batch
build.bat
```

2. **Install to VST3 folder:**
```batch
install.bat
```

3. **Load in your DAW** and start using!

## Detailed Build Instructions

### Step 1: Prerequisites

Install the required build tools using winget (Windows Package Manager):

```batch
winget install Git.Git
winget install Kitware.CMake
winget install Microsoft.VisualStudio.2022.BuildTools
```

Or download manually:
- [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) (Build Tools or Community)
- [CMake](https://cmake.org/download/) (3.15+)
- [Git](https://git-scm.com/downloads)

### Step 2: Clone VST3 SDK

If the `vst3sdk` folder is not present, clone it:

```batch
git clone --recursive https://github.com/steinbergmedia/vst3sdk.git
```

### Step 3: Build

Run the automated build script:

```batch
build.bat
```

This will:
- Configure CMake for Visual Studio 2022
- Build the plugin in Release mode
- Run the VST3 validator (47 automated tests)
- Output: `WetDelay\build\VST3\Release\WetDelay.vst3`

### Step 4: Install

To install the plugin to your system's VST3 folder:

```batch
install.bat
```

**Note**: You may need to run as Administrator if you encounter permission errors.

### Manual Installation

Alternatively, copy the built plugin manually:

```batch
xcopy /Y /I WetDelay\build\VST3\Release\WetDelay.vst3 "%COMMONPROGRAMFILES%\VST3\WetDelay.vst3\"
```

## Usage

1. **Load the plugin** in your DAW (Reaper, Cubase, Ableton Live, FL Studio, etc.)
2. **Select delay time** using the Delay Time parameter (0-5 for 6 positions)
3. **Monitor levels** using the built-in input/output meters
4. **Automate** the delay time parameter for creative effects

### Parameter Reference

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Delay Time | 0-5 | 0 | Selects delay time: 0=20ms, 1=40ms, 2=80ms, 3=120ms, 4=220ms, 5=400ms |

### Delay Times

| Position | Delay Time |
|----------|------------|
| 0 | 20 ms |
| 1 | 40 ms |
| 2 | 80 ms |
| 3 | 120 ms |
| 4 | 220 ms |
| 5 | 400 ms |

## Technical Details

### Architecture

- **Framework**: VST3 SDK (Official Steinberg)
- **Language**: C++17
- **Build System**: CMake + MSBuild
- **GUI**: VSTGUI4

### Audio Processing

- **Host Sample Rates**: Supports 22.05 kHz to 384 kHz
- **Internal Sample Rate**: 24 kHz (80s rack-style)
- **Host Bit Depth**: 32-bit float processing
- **Internal Bit Depth**: 12-bit quantization with dither
- **Latency**: User-controlled (20-400ms delay)
- **CPU Usage**: <0.5% (typical)
- **Memory**: ~200 KB

### Implementation Details

- **Delay Engine**: Circular buffer at 24 kHz internal rate
- **Resampling**: Linear interpolation with anti-aliasing and reconstruction filters
- **Quantization**: 12-bit uniform quantization with TPDF dither
- **Noise Floor**: Fixed -80 dBFS analog-style noise
- **Filtering**: 1st-order high-pass (80 Hz) and low-pass (9 kHz)
- **Crosstalk**: 1% (-40 dB) bidirectional channel bleed
- **Metering**: Atomic peak detection with exponential decay
- **Thread Safety**: Lock-free atomic operations for GUI communication
- **Buffer Size**: Pre-allocated for 400ms @ internal sample rate

## Project Structure

```
WetDelay/
├── vst3sdk/                    # VST3 SDK (git submodule)
├── WetDelay/                   # Plugin source
│   ├── source/
│   │   ├── wetdelayprocessor.h/cpp    # Audio processing
│   │   ├── wetdelaycontroller.h/cpp   # Parameter control
│   │   ├── delaybuffer.h/cpp          # Delay buffer implementation
│   │   ├── wetdelaycids.h             # Plugin IDs
│   │   └── version.h                  # Version info
│   ├── resource/
│   │   └── wetdelayeditor.uidesc      # GUI definition
│   ├── CMakeLists.txt                 # Build configuration
│   └── build/                         # Build output (generated)
├── build.bat                   # Build automation script
├── install.bat                 # Installation script
├── LICENSE                     # MIT License
└── README.md                   # This file
```

## Validation Results

The plugin passes all official VST3 validation tests:

✅ **47 tests passed, 0 tests failed**

Key validations:
- Valid state transitions
- Proper bus configuration
- Correct parameter handling
- Sample rate support (22.05 kHz - 384 kHz)
- Thread safety
- Preset save/load
- Plugin suspend/resume

## Troubleshooting

### Build Errors

**CMake configuration failed:**
- Ensure Visual Studio 2022 is installed
- Verify CMake is in your PATH
- Check that vst3sdk exists in the correct location

**MSBuild error:**
- Make sure Visual Studio 2022 Build Tools are installed
- Try running from "Developer Command Prompt for VS 2022"

### Installation Issues

**Symlink error (exit code 1):**
- This is normal - the plugin is still built successfully
- Run `install.bat` to copy the plugin manually
- Or run the script as Administrator for symlink support

**Plugin not appearing in DAW:**
- Restart your DAW after installation
- Check VST3 scan path: `%COMMONPROGRAMFILES%\VST3\`
- Verify the folder contains `WetDelay.vst3`

### Runtime Issues

**No sound output:**
- Verify the plugin is receiving audio input
- Check that the delay time is not set to minimum (varies by sample rate)
- Ensure your DAW is routing through the plugin correctly

**Crackling/Clicking:**
- This shouldn't occur with discrete parameter changes
- If it does, report as a bug with your DAW and sample rate info

## Development

### Building from Source

If you want to modify the plugin:

1. Edit source files in `WetDelay/source/`
2. Run `build.bat` to rebuild
3. The plugin automatically rebuilds and validates

### Testing

The build process automatically runs:
- **moduleinfotool**: Generates plugin metadata
- **validator**: Runs 47 comprehensive tests

Test coverage includes:
- Parameter automation
- State save/restore
- Multi-threading
- Different buffer sizes
- Various sample rates

## Author

**Ronald Klarenbeek**
- Website: [https://wetvst.com](https://wetvst.com)
- Email: contact@wetvst.com
- GitHub: [https://github.com/yonie](https://github.com/yonie)

## License

MIT License - Copyright © 2026 Ronald Klarenbeek (Yonie)

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

**Note:** This project uses the VST3 SDK which is licensed under a BSD-style license.
See the VST3 SDK license files for details on SDK licensing.

## Acknowledgments

- Steinberg Media Technologies for the VST3 SDK
- VSTGUI framework for cross-platform GUI support
- The audio plugin development community

## Version History

### v1.2.0 (2026-01-09)
- **Channel Crosstalk**:
  - Authentic -40 dB (1%) L/R channel bleed
  - Simulates analog circuitry imperfections
  
### v1.1.0 (2026-01-09)
- **80s Rack-Style Character**:
  - 24 kHz internal sample rate with resampling
  - 12-bit quantization (4096 levels)
  - TPDF dither for smooth quantization
  - Fixed -80 dBFS noise floor
  - Anti-aliasing and reconstruction filters

### v1.0.0 (2026-01-07)
- Initial release
- Core stereo delay functionality
- 6 fixed delay times (20-400ms)
- 100% wet output
- Input/output peak metering
- Full VST3 automation support
- Validated with official VST3 validator

---

**Built with ❤️ and precision engineering**

## Support

If you find this plugin helpful, consider buying me a coffee!

[![Buy Me A Coffee](https://img.shields.io/badge/Buy%20Me%20A%20Coffee-support-yellow?style=flat&logo=buy-me-a-coffee)](https://buymeacoffee.com/yonie)
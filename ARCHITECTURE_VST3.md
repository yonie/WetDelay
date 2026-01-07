# WET Delay VST3 Plugin - Revised Architecture (VST3 Project Generator)

## Technology Stack - REVISED

**Framework**: VST3 SDK + VST3 Project Generator
- Official Steinberg tooling
- Generates clean CMake-based project structure
- Native VST3 implementation (no wrapper overhead)
- VSTGUI4 for native VST3 UI
- Command-line build via CMake + MSBuild

**Build System**: 
- CMake 3.15+ (generates Visual Studio 2022 project)
- MSBuild via command line
- Python 3.x (for project generator script)

## Setup Process (Simplified)

### Prerequisites
```batch
# 1. Git (to clone repos)
# 2. Python 3.x
# 3. CMake 3.15+
# 4. Visual Studio 2022 Build Tools
```

### Quick Setup
```batch
# Clone VST3 SDK
git clone --recursive https://github.com/steinbergmedia/vst3sdk.git

# Clone Project Generator
git clone https://github.com/steinbergmedia/vst3projectgenerator.git

# Generate WetDelay project
cd vst3projectgenerator
python3 project_generator.py WetDelay --vendor Yonie --output ../wetdelay

# Configure & Build
cd ../wetdelay
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

## Project Structure

```
g:/code/plugin/
├── vst3sdk/                     # VST3 SDK (from git clone)
├── vst3projectgenerator/        # Generator tool
├── wetdelay/                    # Our plugin project
│   ├── CMakeLists.txt           # Build configuration
│   ├── source/
│   │   ├── wetdelay_processor.h
│   │   ├── wetdelay_processor.cpp
│   │   ├── wetdelay_controller.h
│   │   ├── wetdelay_controller.cpp
│   │   ├── wetdelay_vstgui.cpp  # GUI editor
│   │   ├── delay_buffer.h       # Custom delay implementation
│   │   ├── delay_buffer.cpp
│   │   └── version.h
│   ├── resource/
│   │   ├── wetdelay.uidesc      # VSTGUI4 UI description
│   │   └── background.png       # Brushed metal texture
│   └── build/
│       └── VST3/
│           └── Release/
│               └── WetDelay.vst3
├── build.bat                    # Automated CMake build
├── install.bat                  # Install to VST3 folder
└── README.md
```

## VST3 Architecture Components

### 1. Processor (Audio Processing)

**WetDelayProcessor** extends `AudioEffect`:
- Handles audio processing in `process()`
- Manages delay buffer state
- Implements parameter changes
- Provides state save/restore

**Key Methods:**
```cpp
class WetDelayProcessor : public AudioEffect {
public:
    tresult PLUGIN_API process(ProcessData& data) override;
    tresult PLUGIN_API setState(IBStream* state) override;
    tresult PLUGIN_API getState(IBStream* state) override;
    
private:
    DelayBuffer delayBuffer;
    int32 currentDelayIndex = 0;
    
    // Metering atomics
    std::atomic<float> inputPeakL{0};
    std::atomic<float> inputPeakR{0};
    std::atomic<float> outputPeakL{0};
    std::atomic<float> outputPeakR{0};
};
```

### 2. Controller (Parameters & GUI)

**WetDelayController** extends `EditController`:
- Defines plugin parameters
- Creates and manages GUI editor
- Handles parameter notifications

**Parameters:**
```cpp
enum WetDelayParams {
    kDelayTimeParam = 0,    // 0-5 (6 positions)
    kParamCount
};
```

**Parameter Setup:**
```cpp
parameters.addParameter(
    STR16("Delay Time"),
    nullptr,
    6,  // stepCount (0-5 = 6 positions)
    0,  // defaultNormalizedValue
    ParameterInfo::kCanAutomate,
    kDelayTimeParam
);
```

### 3. Delay Buffer Implementation

**DelayBuffer class** (same concept as JUCE plan):
```cpp
class DelayBuffer {
public:
    void prepare(double sampleRate, int32 maxDelayMs);
    void processStereo(float* left, float* right, int32 numSamples, int32 delayMs);
    void reset();
    
private:
    std::vector<float> bufferL;
    std::vector<float> bufferR;
    int32 writePos;
    int32 maxSamples;
};
```

**Delay Time Mapping:**
```cpp
const int32 DELAY_TIMES_MS[] = {20, 40, 80, 120, 220, 400};

int32 getDelayTimeMs(int32 paramIndex) {
    return DELAY_TIMES_MS[paramIndex];
}
```

### 4. VSTGUI4 Interface

**GUI Editor** using VSTGUI4:
- XML-based UI description (.uidesc file)
- CView hierarchy for components
- Custom CView subclasses for meters
- CTextButton or CKickButton for delay selection

**Custom Components:**

1. **LevelMeterView** (extends CControl):
```cpp
class LevelMeterView : public CControl {
public:
    void setLevel(float level);  // 0.0 to 1.0
    void draw(CDrawContext* context) override;
    
private:
    float currentLevel = 0.0f;
    static constexpr int32 NUM_SEGMENTS = 12;
};
```

2. **Background with texture**:
```cpp
// In VSTGUI editor
auto background = new CView(CRect(0, 0, 700, 400));
background->setBackground(new CBitmap("background.png"));
```

### 5. UI Layout (VSTGUI4)

**Main View (700x400):**
```
┌─────────────────────────────────────────────────────┐
│                                                     │
│  [InputMeterL]  [InputMeterR]    [OutputMeterL] [OutputMeterR]
│      INPUT L R                      OUTPUT L R     │
│                                                     │
│                  WET DELAY                         │
│                                                     │
│  [20ms] [40ms] [80ms] [120ms] [220ms] [400ms]    │
│   BTN1   BTN2   BTN3   BTN4    BTN5    BTN6       │
└─────────────────────────────────────────────────────┘
```

**Component Positions:**
- Background: (0, 0, 700, 400)
- Input Meter L: (50, 60, 65, 200)
- Input Meter R: (80, 60, 95, 200)
- Output Meter L: (370, 60, 385, 200)
- Output Meter R: (400, 60, 415, 200)
- Delay Buttons: y=280, starting x=40, width=80, spacing=110

**Colors:**
- Background: Dark grey #2c2c2c
- Meter Green: #00ff00
- Meter Yellow: #ffff00
- Meter Red: #ff0000
- Text: Light grey #d0d0d0

## Build Configuration (CMakeLists.txt)

```cmake
cmake_minimum_required(VERSION 3.15)

project(WetDelay)

set(SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../vst3sdk")

# Add SDK subdirectory
add_subdirectory(${SDK_ROOT} ${PROJECT_BINARY_DIR}/sdk)

# Plugin target
smtg_add_vst3plugin(WetDelay
    source/wetdelay_processor.cpp
    source/wetdelay_processor.h
    source/wetdelay_controller.cpp
    source/wetdelay_controller.h
    source/wetdelay_vstgui.cpp
    source/delay_buffer.cpp
    source/delay_buffer.h
)

# Set plugin properties
smtg_target_set_plugin_name(WetDelay "WetDelay")
smtg_target_set_plugin_vendor(WetDelay "Yonie")
smtg_target_set_plugin_category(WetDelay Fx)

# Link to VSTGUI
target_link_libraries(WetDelay PRIVATE
    sdk
    vstgui_support
)

# Copy resources
smtg_target_add_plugin_resources(WetDelay
    RESOURCES
        resource/wetdelay.uidesc
        resource/background.png
)
```

## Implementation Strategy

### Phase 1: Project Setup ✅
1. Clone VST3 SDK with submodules
2. Clone VST3 Project Generator
3. Generate WetDelay project skeleton
4. Verify CMake configuration builds

### Phase 2: Core Audio (Processor)
1. Implement DelayBuffer class
2. Add delay parameter to processor
3. Implement process() with 100% wet delay
4. Add peak metering in process loop

### Phase 3: Controller & Parameters
1. Define delay time parameter (0-5 discrete)
2. Implement parameter -> delay time mapping
3. Add parameter change handling
4. Implement state save/restore

### Phase 4: VSTGUI4 Interface
1. Create .uidesc XML file with layout
2. Implement custom LevelMeterView
3. Create 6 toggle buttons with radio behavior
4. Add background image and text labels
5. Wire parameter changes to UI updates

### Phase 5: Build & Test
1. Create build.bat (CMake + MSBuild automation)
2. Create install.bat (copy to VST3 folder)
3. Test in VST3 host (Reaper, Cubase, etc.)
4. Validate all parameters and metering

## Signal Flow (Same as Before)

```
Audio Input L/R
  ↓
Input Peak Detection → GUI Meters
  ↓
Write to Delay Buffer
  ↓
Read from Buffer (offset by delay time)
  ↓
Output Peak Detection → GUI Meters
  ↓
Audio Output L/R (100% wet)
```

## Parameter Automation

VST3 handles automation via `IEditController`:
```cpp
// In controller
tresult PLUGIN_API setParamNormalized(ParamID tag, ParamValue value) {
    if (tag == kDelayTimeParam) {
        // Update processor via message queue
        // GUI will update automatically via notification
    }
    return EditController::setParamNormalized(tag, value);
}
```

## Performance

- **Memory**: ~200 KB (mostly delay buffer)
- **CPU**: <0.5% (simple circular buffer operations)
- **Latency**: User-selectable (20-400ms)

## Build Scripts

**build.bat:**
```batch
@echo off
echo Building WetDelay VST3...

cd wetdelay

if not exist build mkdir build
cd build

cmake .. -G "Visual Studio 17 2022" -A x64
if errorlevel 1 goto error

cmake --build . --config Release
if errorlevel 1 goto error

echo Build successful!
copy /Y VST3\Release\WetDelay.vst3 ..\..\Build\
goto end

:error
echo Build failed!
exit /b 1

:end
```

**install.bat:**
```batch
@echo off
set VST3_PATH=%COMMONPROGRAMFILES%\VST3
copy /Y Build\WetDelay.vst3 "%VST3_PATH%\"
echo Installed to %VST3_PATH%\WetDelay.vst3
```

## Advantages Over JUCE

1. **Lightweight**: Native VST3, no framework overhead
2. **Official**: Steinberg's own tools and SDK
3. **Simple**: Project generator creates clean template
4. **CMake**: Industry-standard build system
5. **VSTGUI4**: Native VST3 UI framework, well documented

## Key Files to Implement

1. `wetdelay_processor.cpp` - Audio processing & delay
2. `wetdelay_controller.cpp` - Parameters & GUI creation
3. `wetdelay_vstgui.cpp` - VSTGUI editor implementation
4. `delay_buffer.cpp` - Circular buffer for stereo delay
5. `wetdelay.uidesc` - VSTGUI XML UI description
6. `background.png` - Brushed metal texture
7. `CMakeLists.txt` - Build configuration

## Testing Checklist

- [ ] Plugin loads in VST3 host
- [ ] All 6 delay times work correctly
- [ ] Input meters show accurate levels
- [ ] Output meters show accurate levels
- [ ] 100% wet output (no dry signal)
- [ ] Parameter automation works in DAW
- [ ] State save/restore works correctly
- [ ] No audio glitches on parameter changes

## Next Steps

Now ready to:
1. Clone repositories
2. Run project generator
3. Implement source files
4. Build and test

This approach is simpler and more native than JUCE!
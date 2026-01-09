# WetDelay UI Implementation Plan
## 80's Style LED Meters and Delay Controls

### Project Overview
Implement vintage 80's-style LED meters and delay time buttons on the WetDelay VST3 plugin UI, precisely positioned over the existing backplate.jpg background image.

---

## 1. Background Image Analysis

**Image Specifications:**
- Dimensions: 702 × 702 pixels
- Format: JPEG background with pre-rendered graphics
- Current uidesc viewport: 400 × 300 (needs updating)

**Visual Elements to Overlay:**

### LED Meter Positions (measured from backplate.jpg)
```
INPUT L/R Meters:
- Location: Top-left section
- Container box: ~32, 85, 325, 160
- Label "INPUT L R" above meters
- Each meter: 12 LED segments (green → yellow → red)
- Segment size: ~18×16 pixels per LED
- Left channel (L): segments arranged horizontally
- Right channel (R): segments arranged horizontally below L

OUTPUT L/R Meters:
- Location: Top-right section  
- Container box: ~377, 85, 670, 160
- Label "OUTPUT L R" above meters
- Layout: Same as INPUT (12 segments × 2 channels)
```

### Delay Button Positions
```
Button Layout: 6 buttons in horizontal row
Y-position: ~455 (top of buttons)
Button dimensions: ~90 × 130 pixels each

Button Coordinates (x, y, width, height):
1. 20ms  : 35,  455, 90, 130
2. 40ms  : 145, 455, 90, 130  
3. 80ms  : 255, 455, 90, 130
4. 120ms : 365, 455, 90, 130
5. 220ms : 475, 455, 90, 130
6. 400ms : 585, 455, 90, 130

Labels above each button display delay time
```

---

## 2. VST3 Architecture for Realtime Meters

### Current State
✅ Processor already tracks peak values:
- `std::atomic<float> inputPeakL`, `inputPeakR`
- `std::atomic<float> outputPeakL`, `outputPeakR`
- Updated in `process()` function

### Required Changes

#### A. Add Meter Parameters (wetdelaycids.h)
```cpp
enum WetDelayParams : Steinberg::Vst::ParamID
{
    kDelayTimeParam = 0,  // Existing
    
    // Output-only meter parameters
    kInputMeterL = 1,
    kInputMeterR = 2,
    kOutputMeterL = 3,
    kOutputMeterR = 4,
    
    kParamCount = 5
};
```

#### B. Processor: Export Meter Values (wetdelayprocessor.h/.cpp)
Add to `process()` function after meter update:
```cpp
// Export meter values to UI via output parameters
if (data.outputParameterChanges)
{
    int32 index = 0;
    auto* paramQueue = data.outputParameterChanges->addParameterData(
        kInputMeterL, index);
    if (paramQueue)
        paramQueue->addPoint(0, inputPeakL.load(), index);
    
    // Repeat for inputPeakR, outputPeakL, outputPeakR
}
```

#### C. Controller: Register Meter Parameters (wetdelaycontroller.cpp)
```cpp
// In initialize():
parameters.addParameter(STR16("Input Meter L"), nullptr, 0, 0,
    Vst::ParameterInfo::kIsReadOnly, kInputMeterL);
parameters.addParameter(STR16("Input Meter R"), nullptr, 0, 0,
    Vst::ParameterInfo::kIsReadOnly, kInputMeterR);
parameters.addParameter(STR16("Output Meter L"), nullptr, 0, 0,
    Vst::ParameterInfo::kIsReadOnly, kOutputMeterL);
parameters.addParameter(STR16("Output Meter R"), nullptr, 0, 0,
    Vst::ParameterInfo::kIsReadOnly, kOutputMeterR);
```

#### D. Controller: Implement Timer for UI Updates
Add to `wetdelaycontroller.h`:
```cpp
#include "vstgui/lib/controls/ccontrol.h"

class WetDelayProcessorController : public EditControllerEx1,
                                     public VSTGUI::ITimerCallback
{
public:
    // ... existing methods ...
    
    // ITimerCallback
    void onTimer(VSTGUI::CVSTGUITimer* timer) override;
    
protected:
    VSTGUI::SharedPointer<VSTGUI::CVSTGUITimer> timer;
};
```

Implement in `.cpp`:
```cpp
// In createView(): Start 50ms timer
timer = VSTGUI::makeOwned<VSTGUI::CVSTGUITimer>(
    [this](VSTGUI::CVSTGUITimer*) { onTimer(nullptr); }, 50);

// In onTimer(): Trigger UI refresh
void WetDelayProcessorController::onTimer(VSTGUI::CVSTGUITimer*)
{
    // VSTGUI will automatically update controls bound to parameters
    // No manual code needed - parameters drive the UI
}
```

---

## 3. 80's LED Meter Implementation

### Design Specification

**Classic LED Characteristics:**
- Discrete rectangular segments (not smooth bars)
- 12 segments per channel
- Color zones:
  - Segments 1-8: Green (#00ff00)
  - Segments 9-10: Yellow (#ffff00)
  - Segments 11-12: Red (#ff0000)
- Off state: Dark gray (#2a2a2a)
- Segment spacing: 2-3 pixels between LEDs
- Peak hold: Optional (200ms hold time)

### VSTGUI Implementation Options

**Option A: VSTGUI CLevelMeter**
```xml
<view class="CViewContainer" ...>
  <view class="CLevelMeter" 
        min-value="0" max-value="1"
        num-led="12"
        origin="32, 100" size="280, 16"
        control-tag="kInputMeterL"
        background-color="#2a2a2a"
        />
</view>
```

**Option B: Custom View (Recommended for Authenticity)**
Create `LEDMeterView` class extending `CControl`:
```cpp
class LEDMeterView : public VSTGUI::CControl
{
public:
    void draw(VSTGUI::CDrawContext* context) override
    {
        float level = getValueNormalized();
        int litSegments = (int)(level * 12);
        
        for (int i = 0; i < 12; i++)
        {
            VSTGUI::CRect segRect = getSegmentRect(i);
            VSTGUI::CColor color = (i < litSegments) ?
                getSegmentColor(i) : offColor;
            
            context->setFillColor(color);
            context->drawRect(segRect, VSTGUI::kDrawFilled);
        }
    }
    
private:
    VSTGUI::CColor getSegmentColor(int segment)
    {
        if (segment < 8) return greenLED;
        if (segment < 10) return yellowLED;  
        return redLED;
    }
    
    VSTGUI::CColor greenLED{0, 255, 0, 255};
    VSTGUI::CColor yellowLED{255, 255, 0, 255};
    VSTGUI::CColor redLED{255, 0, 0, 255};
    VSTGUI::CColor offColor{42, 42, 42, 255};
};
```

---

## 4. Delay Button Implementation

### Design Requirements
- 6 mutually exclusive buttons (radio group)
- Visual feedback for selected state
- Click to activate
- Map to kDelayTimeParam values (0.0, 0.2, 0.4, 0.6, 0.8, 1.0)

### VSTGUI Implementation

**Option A: CCheckBox with Custom Graphics**
```xml
<view class="CCheckBox"
      control-tag="kDelayTimeParam"
      origin="35, 455" size="90, 130"
      on-value="0.0" off-value="0.0"
      background-offset="0, 0"
      />
```

**Option B: CTextButton (Recommended)**
```xml
<view class="CTextButton"
      control-tag="kDelayTimeParam"
      title="20ms"
      origin="35, 455" size="90, 130"
      font="Arial" font-size="12"
      text-color="#ffffff"
      frame-color="#666666"
      back-color-on="#00ff00"
      back-color-off="#1a1a1a"
      value="0.0"
      />
```

**Option C: Radio Button Group Controller**
Custom subController to handle mutual exclusivity:
```cpp
class DelayButtonController : public IControlListener
{
    void valueChanged(CControl* control) override
    {
        // When any button pressed, deactivate others
        // Set kDelayTimeParam to corresponding value
    }
};
```

---

## 5. UIDESC XML Structure

### Complete Template
```xml
<?xml version="1.0" encoding="UTF-8"?>
<vstgui-ui-description version="1">
    <colors>
        <color name="White" rgba="#ffffffff"/>
        <color name="Black" rgba="#000000ff"/>
        <color name="LEDGreen" rgba="#00ff00ff"/>
        <color name="LEDYellow" rgba="#ffff00ff"/>
        <color name="LEDRed" rgba="#ff0000ff"/>
        <color name="LEDOff" rgba="#2a2a2aff"/>
    </colors>
    
    <template background-color="~ BlackCColor" 
              bitmap="backplate" 
              class="CViewContainer" 
              name="view" 
              size="702, 702">
        
        <!-- Input Meter Left -->
        <view class="CLevelMeter" 
              control-tag="kInputMeterL"
              origin="50, 100" size="250, 16"
              num-led="12" />
        
        <!-- Input Meter Right -->
        <view class="CLevelMeter"
              control-tag="kInputMeterR"
              origin="50, 125" size="250, 16"
              num-led="12" />
        
        <!-- Output Meter Left -->
        <view class="CLevelMeter"
              control-tag="kOutputMeterL"
              origin="395, 100" size="250, 16"
              num-led="12" />
        
        <!-- Output Meter Right -->
        <view class="CLevelMeter"
              control-tag="kOutputMeterR"
              origin="395, 125" size="250, 16"
              num-led="12" />
        
        <!-- Delay Buttons -->
        <view class="CTextButton" title="20ms"
              control-tag="kDelayTimeParam"
              origin="35, 455" size="90, 130" />
        <view class="CTextButton" title="40ms"
              control-tag="kDelayTimeParam"
              origin="145, 455" size="90, 130" />
        <!-- ... repeat for all 6 buttons ... -->
    </template>
    
    <bitmaps>
        <bitmap name="backplate" path="backplate.jpg"/>
    </bitmaps>
</vstgui-ui-description>
```

---

## 6. Implementation Workflow

### Phase 1: Parameter Communication
1. ✅ Update [`wetdelaycids.h`](WetDelay/source/wetdelaycids.h) - Add meter parameter IDs
2. ✅ Modify [`wetdelayprocessor.cpp`](WetDelay/source/wetdelayprocessor.cpp) - Export meter values
3. ✅ Modify [`wetdelaycontroller.cpp`](WetDelay/source/wetdelaycontroller.cpp) - Register meter parameters

### Phase 2: UI Controls
4. ✅ Update [`wetdelayeditor.uidesc`](WetDelay/resource/wetdelayeditor.uidesc) - Add all UI elements
5. ✅ Adjust viewport size from 400×300 to 702×702
6. ✅ Position 4 LED meters at measured coordinates
7. ✅ Position 6 delay buttons at measured coordinates

### Phase 3: Visual Refinement
8. ✅ Configure LED meter appearance (colors, segments)
9. ✅ Configure button appearance (text, colors, feedback)
10. ✅ Test alignment with background image

### Phase 4: Testing
11. ✅ Verify meter responsiveness to audio signal
12. ✅ Verify button mutual exclusivity
13. ✅ Verify parameter automation works
14. ✅ Test in DAW environment

---

## 7. Technical Considerations

### VST3 Output Parameters
- Marked as `kIsReadOnly` and `kNoFlags`
- Updated in audio processing thread
- UI reads values via parameter change notifications
- No need for manual synchronization (VST3 handles it)

### Timer Performance
- 50ms refresh = 20 fps (adequate for meters)
- Lower CPU usage than per-buffer updates
- Matches human perception (~25-30 fps smooth)

### LED Segment Count
- 12 segments = 6dB per segment (assuming -72dB to 0dB range)
- 0 dB = all 12 lit
- -72 dB = none lit
- Logarithmic scaling required for proper display

### Coordinate System
- VSTGUI uses top-left origin (0,0)
- Coordinates are absolute within template
- Background image auto-scales with viewport

---

## 8. Alternative Approaches

### Approach A: VSTGUI Built-in Controls
**Pros:**
- Faster implementation
- Less custom code
- Automatic parameter binding

**Cons:**
- Less control over 80's aesthetic
- May not match vintage look exactly

### Approach B: Custom View Classes
**Pros:**
- Full control over rendering
- Perfect 80's LED emulation
- Custom animation/effects

**Cons:**
- More C++ code required
- Longer development time
- Need to register custom view factories

### Recommendation
**Hybrid approach:**
- Use VSTGUI `CLevelMeter` with custom styling
- Use `CTextButton` for delay controls
- Override colors/appearance via XML
- Add custom views only if needed

---

## 9. Success Criteria

✅ **Visual:**
- Meters align perfectly with background graphics
- LED segments look authentic (discrete, not smooth)
- Buttons align with background rectangles
- Active button shows clear visual feedback

✅ **Functional:**
- Meters respond to audio signal in realtime
- Meters show proper peak hold and decay
- Only one delay button active at a time
- Button clicks change delay time immediately
- Parameter automation works correctly

✅ **Performance:**
- UI updates smoothly (20+ fps)
- No audio glitches from UI activity
- CPU usage remains minimal

---

## 10. Future Enhancements

### v2.0 Features
- [ ] Peak hold indicators on meters
- [ ] Clip indicators (red LED persistent)
- [ ] Button LED indicator (small light when active)
- [ ] Keyboard shortcuts for delay selection
- [ ] MIDI learn for buttons
- [ ] Custom skin/color themes
- [ ] Resizable UI with scaling

### Advanced Features
- [ ] VU-style meter ballistics option
- [ ] PPM meter mode (BBC standard)
- [ ] Stereo correlation meter
- [ ] Spectrum analyzer overlay
- [ ] Waveform display

---

## Conclusion

This implementation plan provides a complete roadmap for adding authentic 80's-style LED meters and delay time controls to the WetDelay VST3 plugin. The architecture leverages VST3's parameter system for realtime communication between the audio processor and UI, ensuring thread-safe and efficient updates.

**Estimated Implementation Time:**
- Phase 1 (Parameters): 30 minutes
- Phase 2 (UI Controls): 1 hour  
- Phase 3 (Refinement): 30 minutes
- Phase 4 (Testing): 30 minutes
- **Total**: ~2.5 hours

**Code Changes Required:**
- [`wetdelaycids.h`](WetDelay/source/wetdelaycids.h) - Add 4 meter parameter IDs
- [`wetdelayprocessor.cpp`](WetDelay/source/wetdelayprocessor.cpp) - Export meter values
- [`wetdelaycontroller.h`](WetDelay/source/wetdelaycontroller.h) - Add timer interface
- [`wetdelaycontroller.cpp`](WetDelay/source/wetdelaycontroller.cpp) - Register meters, setup timer
- [`wetdelayeditor.uidesc`](WetDelay/resource/wetdelayeditor.uidesc) - Complete UI layout

Ready to proceed with implementation!
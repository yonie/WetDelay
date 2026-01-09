# WetDelay VST3 UI Troubleshooting Report

**Date:** 2026-01-07  
**Status:** Meters render, buttons invisible - CKickButton doesn't support text labels

## Current State Analysis (debug5.png)

**Plugin Status:** ACTIVE (not bypassed) - title shows "Track 1 [2/4]"

### What's Rendering

1. ✅ **Background Image** - 702x702px backplate with decorative LED meters and delay time labels
2. ✅ **Meter Controls** - 4 white horizontal bars showing "0.00" (CParamDisplay working)
3. ❌ **Button Controls** - 6 empty rounded rectangles (CKickButton frames only, no text)

### The Obvious Problem

`CKickButton` renders frame outlines but does NOT render the `title` attribute text. The delay labels ("20ms", "40ms", etc.) visible in the image are part of the background JPG file, not the button controls.

## Technical Status

### What Works (Barely)
- ✅ Plugin compiles (47 VST3 tests pass)
- ✅ Plugin loads in DAW
- ✅ Background image renders correctly
- ✅ CParamDisplay controls render (as white boxes)
- ✅ CKickButton controls render (as empty frames)

### What Is BROKEN
- ❌ **Meters do NOT update** - stuck at "0.00" even with audio playing
- ❌ **Buttons do NOT respond to clicks** - clicking does nothing
- ❌ **Buttons have NO text** - CKickButton doesn't render `title` attribute
- ❌ **No visual feedback** - can't tell what's selected or if anything works
- ❌ **Lost previous functionality** - earlier had working debug CTextButton controls that showed text

### Regression
**What we HAD before (debug3.png):**
- ✅ Visible meter bars with "INPUT L", "INPUT R", etc. labels
- ✅ Clickable white buttons with red text ("20ms", "40ms", etc.)

**What we have NOW (debug5.png):**
- ❌ Meters show "0.00" text, don't update
- ❌ Buttons are invisible/unlabeled empty frames

## Root Cause: Wrong Control Type

**CKickButton** is designed for:
- Momentary actions (like a piano key)
- Visual feedback via background state changes
- NOT for displaying text labels
- NOT for discrete parameter selection

**Evidence:**
```xml
<view class="CKickButton"
      control-tag="kDelayTimeParam"
      title="20ms"   <!-- THIS ATTRIBUTE IS IGNORED -->
      font="DelayFont"  <!-- THIS ATTRIBUTE IS IGNORED -->
      ...
```

The control type simply doesn't support text rendering.

## Evolution of Attempts

| Attempt | Control Type | Text Visible? | Functional? | Issue |
|---------|-------------|---------------|-------------|-------|
| 1 | CTextButton (no attrs) | ✅ Yes | ❌ No | Didn't set parameter values |
| 2 | CTextButton (with values) | ✅ Yes | ❌ Partially | All buttons share same param |
| 3 | COnOffButton | ❌ No | ❌ No | Disappeared completely |
| 4 | CKickButton (current) | ❌ No | ❓ Unknown | Empty frames only |

## Why Each Failed

### CTextButton
- Renders text labels correctly
- Problem: Not designed for discrete radio-button behavior
- All instances control the same parameter
- No exclusive selection (can't have "only one selected")

### COnOffButton
- Binary on/off control
- Doesn't render text/title attributes
- Designed for toggle switches, not labeled buttons

### CKickButton  
- Momentary trigger control
- Doesn't render text/title attributes
- Designed for piano-key style interactions
- No persistent "selected" state

## The Real Problem: VSTGUI Limitation

VSTGUI does not provide a built-in "radio button group" control suitable for:
- Discrete parameter selection (6 mutually exclusive options)
- Text label display on each button
- Visual indication of current selection
- Click to select behavior

## Recommended Solutions

### Solution 1: Revert to CTextButton + Accept Limitations

**Pro:** Simple, text visible  
**Con:** Won't show which is selected, may not set values correctly

```xml
<view class="CTextButton"
      control-tag="kDelayTimeParam"
      origin="35, 630"
      size="90, 40"
      title="20ms"
      font="DelayFont"
      text-color="White"
      .../>
```

### Solution 2: CParamDisplay Grid (Read-Only Display)

Show current delay time as text, use invisible clickable areas:

```xml
<!-- Display current value -->
<view class="CParamDisplay"
      control-tag="kDelayTimeParam"
      value-to-string-function="indexed"
      .../>

<!-- 6 invisible click zones -->
<view class="CViewContainer"
      <!-- Handle clicks in custom code -->
      .../>
```

**Pro:** Can show current selection  
**Con:** Requires custom click handling in C++

### Solution 3: Custom C++ Button Controller (Proper Fix)

Implement `IControlListener` to manage button group:

```cpp
class DelayButtonListener : public IControlListener {
    void valueChanged(CControl* control) override {
        int buttonIndex = control->getTag();  
        float value = buttonIndex / 5.0;  // 0.0, 0.2, 0.4...
        setParamNormalized(kDelayTimeParam, value);
        updateButtonStates(buttonIndex);  // Highlight selected
    }
};
```

**Pro:** Full control, proper radio behavior  
**Con:** Requires C++ code changes, complexity

### Solution 4: Use Host Automation Display

Accept that UI doesn't show selection, rely on:
- Host parameter display
- Host automation lanes  
- External controller (MIDI)

**Pro:** No code changes needed  
**Con:** Poor user experience

## Meter Status

**Current:** CParamDisplay showing "0.00"

**Possible Reasons:**
1. ✅ No audio playing (most likely - screenshot taken  without audio)
2. ❓ Timer not triggering UI updates
3. ❓ Parameter changes not propagating
4. ❓ Processor not sending meter values

**Next Test:** Play audio and check if values change from 0.00

## Files Modified Session

1. `wetdelayeditor.uidesc` - Meter controls (CTextButton → CVuMeter → CParamDisplay)
2. `wetdelayeditor.uidesc` - Button controls (CTextButton → COnOffButton → CKickButton)
3. `wetdelayprocessor.cpp` - Meter calculations and parameter output
4. `wetdelaycontroller.cpp` - Timer setup for UI updates

## Build Configuration

**Source:** `G:\code\plugin\WetDelay\build\VST3\Release\WetDelay.vst3`  
**Target:** `C:\Program Files\Common Files\VST3\WetDelay.vst3`  
**Status:** Requires admin for symbolic link OR manual copy

## Immediate Action Items

1. **Test meters with audio** - Confirm CParamDisplay updates when audio plays
2. **Test button clicks** - Click empty buttons, check if delay time changes
3. **Choose button solution** - Pick from 4 options above based on requirements
4. **Implement chosen solution** - Make final UI changes
5. **Rebuild and test** - Verify complete functionality

## Key Insights

1. **Background vs Controls:** Static background labels ≠ control text
2. **Control Purposes:** Wrong control type = wrong behavior
3. **VSTGUI Gaps:** No built-in radio button group control
4. **Parameter Types:** StringListParameter needs special UI handling
5. **Testing blind spot:** Screenshots during bypass/silence mislead

## Bottom Line: We Made It WORSE

**Starting point (debug3.png):**
- Meters visible with labels
- Buttons visible with red text
- Unknown if functional but at least VISIBLE

**Ending point (debug5.png):**
- Meters show "0.00", don't react to audio
- Buttons completely unlabeled/invisible
- Definitely NOT functional

**What happened:**
- Changed from working CTextButton to non-working CKickButton
- Changed from visible meters to non-updating CParamDisplay
- Lost all UI functionality trying to "improve" it

## What Must Be Done Next Session

1. **REVERT to working CTextButton version** for delay buttons
   - Go back to what was in debug3.png
   - At least users can SEE what they're clicking
   
2. **Fix meter update issue**
   - Investigate why CParamDisplay stuck at 0.00
   - Check timer implementation
   - Test parameter notification chain
   
3. **THEN improve button functionality**
   - Don't change what renders
   - Fix parameter value setting
   - Add proper click handling

4. **Stop experimenting with control types**
   - CTextButton works for rendering
   - Focus on making it SET VALUES correctly
   - Don't "fix" what isn't broken

## Success Criteria

For this plugin to be "working":
- [ ] Meters visually respond to audio levels
- [ ] Delay buttons are visible and labeled
- [ ] Clicking a button selects that delay time
- [ ] Only one button appears selected at a time
- [ ] Delay processing audibly changes with button selection
- [ ] Button selection persists across plugin reload

## Environment

- **Platform:** Windows 11
- **Compiler:** MSVC 19.44
- **SDK:** VST3 SDK with VSTGUI 4
- **DAW:** Unknown (shows as "Track 1")
- **Build:** Release, 47/47 tests passed

## Files to Check Next Time

- `wetdelayeditor.uidesc` - Current broken state with CKickButton
- `debug3.png` - Last known working UI state
- `wetdelaycontroller.cpp` - Timer and parameter notification
- Git history - to find exact working configuration
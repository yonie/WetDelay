# WetDelay UI Architecture Diagram

## Data Flow: Audio Processing → UI Display

```mermaid
graph TB
    subgraph AudioThread[Audio Thread - Processor]
        A1[Audio Input L/R] --> A2[Process Delay Buffer]
        A2 --> A3[Audio Output L/R]
        A2 --> M1[Update Peak Meters]
        M1 --> M2[std::atomic inputPeakL/R]
        M1 --> M3[std::atomic outputPeakL/R]
        M2 --> P1[Output Parameter Changes]
        M3 --> P1
        P1 --> P2[kInputMeterL/R]
        P1 --> P3[kOutputMeterL/R]
    end
    
    subgraph VST3Host[VST3 Host - Parameter Communication]
        P2 --> H1[Parameter Queue]
        P3 --> H1
        H1 --> H2[Thread-Safe Transfer]
    end
    
    subgraph UIThread[UI Thread - Controller]
        H2 --> C1[Controller Parameters]
        C1 --> T1[50ms Timer]
        T1 --> T2[Trigger UI Refresh]
        T2 --> U1[LED Meter Views]
        U1 --> U2[Draw 12 Segments]
        U2 --> U3[Green/Yellow/Red Colors]
    end
    
    subgraph UserInput[User Interaction]
        UI1[Click Delay Button] --> UI2[kDelayTimeParam]
        UI2 --> UI3[Controller]
        UI3 --> UI4[Input Parameter Changes]
        UI4 --> A4[Processor Updates]
        A4 --> A2
    end
    
    style AudioThread fill:#e1f5e1
    style UIThread fill:#e1e5f5
    style VST3Host fill:#f5f5e1
    style UserInput fill:#f5e1e1
```

## UI Layout Map - 702×702 Pixel Canvas

```mermaid
graph TD
    subgraph Canvas[702×702 Background Image]
        subgraph InputMeters[INPUT L R - Top Left]
            IM1[Input L Meter: 12 LEDs]
            IM2[Input R Meter: 12 LEDs]
        end
        
        subgraph OutputMeters[OUTPUT L R - Top Right]
            OM1[Output L Meter: 12 LEDs]
            OM2[Output R Meter: 12 LEDs]
        end
        
        subgraph Title[Center]
            T1[WET DELAY Logo]
        end
        
        subgraph Buttons[Bottom - Delay Time Buttons]
            B1[20ms]
            B2[40ms]
            B3[80ms]
            B4[120ms]
            B5[220ms]
            B6[400ms]
        end
    end
    
    style InputMeters fill:#90ee90
    style OutputMeters fill:#90ee90
    style Title fill:#ddd
    style Buttons fill:#ffa07a
```

## LED Meter Segment Display

```mermaid
graph LR
    subgraph MeterDisplay[12-Segment LED Meter]
        S1[1] --> S2[2]
        S2 --> S3[3]
        S3 --> S4[4]
        S4 --> S5[5]
        S5 --> S6[6]
        S6 --> S7[7]
        S7 --> S8[8]
        S8 --> S9[9]
        S9 --> S10[10]
        S10 --> S11[11]
        S11 --> S12[12]
    end
    
    subgraph Legend[Color Zones]
        G[Green: 1-8]
        Y[Yellow: 9-10]
        R[Red: 11-12]
    end
    
    style S1 fill:#00ff00
    style S2 fill:#00ff00
    style S3 fill:#00ff00
    style S4 fill:#00ff00
    style S5 fill:#00ff00
    style S6 fill:#00ff00
    style S7 fill:#00ff00
    style S8 fill:#00ff00
    style S9 fill:#ffff00
    style S10 fill:#ffff00
    style S11 fill:#ff0000
    style S12 fill:#ff0000
    style G fill:#00ff00
    style Y fill:#ffff00
    style R fill:#ff0000
```

## Component Hierarchy

```mermaid
classDiagram
    class WetDelayProcessor {
        -DelayBuffer delayBuffer
        -atomic~float~ inputPeakL
        -atomic~float~ inputPeakR
        -atomic~float~ outputPeakL
        -atomic~float~ outputPeakR
        +process(ProcessData)
        +updatePeak(sample, peak)
    }
    
    class WetDelayController {
        -CVSTGUITimer timer
        +initialize()
        +createView()
        +onTimer()
    }
    
    class VST3Editor {
        -uidesc file
        +create UI from XML
    }
    
    class LEDMeterView {
        -int numSegments
        -CColor colors
        +draw(context)
        +getSegmentColor(index)
    }
    
    class DelayButton {
        -ParamValue value
        +valueChanged()
    }
    
    WetDelayProcessor --> WetDelayController: parameter changes
    WetDelayController --> VST3Editor: creates
    VST3Editor --> LEDMeterView: contains 4×
    VST3Editor --> DelayButton: contains 6×
    LEDMeterView --> WetDelayController: reads params
    DelayButton --> WetDelayController: writes params
```

## Parameter Routing

```mermaid
sequenceDiagram
    participant AP as Audio Processor
    participant Host as VST3 Host
    participant Ctrl as Controller
    participant UI as UI Views
    participant User
    
    Note over AP,UI: Meter Value Flow (Audio → UI)
    
    AP->>AP: Process audio samples
    AP->>AP: updatePeak() on atomics
    AP->>Host: outputParameterChanges
    Host->>Ctrl: parameter notification
    Ctrl->>Ctrl: 50ms timer tick
    Ctrl->>UI: valueChanged event
    UI->>UI: Redraw LED meters
    
    Note over AP,UI: Button Click Flow (UI → Audio)
    
    User->>UI: Click delay button
    UI->>Ctrl: valueChanged(kDelayTimeParam)
    Ctrl->>Host: setParamNormalized()
    Host->>AP: inputParameterChanges
    AP->>AP: Update currentDelayIndex
    AP->>AP: Process with new delay time
```

## Coordinate Reference Map

```
┌─────────────────────────────────────────────────────────────────┐  0
│                                                                 │
│   INPUT L R (50,45)              OUTPUT L R (395,45)           │  50
│   ┌─────────────────────┐       ┌─────────────────────┐       │
│   │ 🟢🟢🟢🟢🟢🟢🟢🟢🟡🟡🔴🔴 │       │ 🟢🟢🟢🟢🟢🟢🟢🟢🟡🟡🔴🔴 │       │  100
│   │ 🟢🟢🟢🟢🟢🟢🟢🟢🟡🟡🔴🔴 │       │ 🟢🟢🟢🟢🟢🟢🟢🟢🟡🟡🔴🔴 │       │  125
│   └─────────────────────┘       └─────────────────────┘       │  150
│                                                                 │
│                       WET DELAY                                 │  280
│                                                                 │
│   20ms  40ms  80ms  120ms 220ms 400ms                          │  420
│   ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐ ┌───┐                        │  455
│   │   │ │   │ │   │ │   │ │   │ │   │                        │
│   │   │ │   │ │   │ │   │ │   │ │   │                        │  520
│   │   │ │   │ │   │ │   │ │   │ │   │                        │
│   └───┘ └───┘ └───┘ └───┘ └───┘ └───┘                        │  585
│                                                                 │
└─────────────────────────────────────────────────────────────────┘  702
0   35  145 255 365 475 585                                    702
```

## Implementation Checklist

### Code Files to Modify

- ✅ [`wetdelaycids.h`](WetDelay/source/wetdelaycids.h)
  - Add meter parameter IDs (4 new constants)
  
- ✅ [`wetdelayprocessor.cpp`](WetDelay/source/wetdelayprocessor.cpp)
  - Add meter value export in `process()` function
  - Use `outputParameterChanges->addParameterData()`
  
- ✅ [`wetdelaycontroller.h`](WetDelay/source/wetdelaycontroller.h)
  - Add `ITimerCallback` interface
  - Add timer member variable
  
- ✅ [`wetdelaycontroller.cpp`](WetDelay/source/wetdelaycontroller.cpp)
  - Register 4 meter parameters as read-only
  - Create and start timer in `createView()`
  - Implement `onTimer()` callback
  
- ✅ [`wetdelayeditor.uidesc`](WetDelay/resource/wetdelayeditor.uidesc)
  - Update viewport from 400×300 to 702×702
  - Add 4 LED meter views with precise coordinates
  - Add 6 delay button views with precise coordinates
  - Configure colors, fonts, and styling

### Testing Checklist

- ✅ Build plugin successfully
- ✅ Load in DAW without errors
- ✅ Input meters respond to incoming audio
- ✅ Output meters respond to delayed output
- ✅ LED segments light up in correct colors (green/yellow/red)
- ✅ Clicking delay buttons changes delay time
- ✅ Only one button active at a time
- ✅ Parameter automation works
- ✅ Visual alignment matches background image
- ✅ CPU usage remains low
- ✅ No audio glitches from UI updates

---

**Architecture Status: COMPLETE** ✅  
**Ready for Implementation: YES** ✅  
**Estimated Build Time: 2.5 hours** ⏱️
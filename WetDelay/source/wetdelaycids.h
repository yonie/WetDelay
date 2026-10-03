//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace Yonie {
//------------------------------------------------------------------------
static const Steinberg::FUID kWetDelayProcessorProcessorUID (0xB4C7B10D, 0xBFB05B3C, 0x9A71DCAD, 0x9E9F2BD9);
static const Steinberg::FUID kWetDelayProcessorControllerUID (0xBDABF70B, 0xE0455D48, 0x8DCD7B40, 0x179AC0D1);

#define WetDelayProcessorVST3Category "Fx|Delay"

//------------------------------------------------------------------------
// Parameter IDs
//------------------------------------------------------------------------
enum WetDelayParams : Steinberg::Vst::ParamID
{
	kDelayTimeParam = 0,  // 0-5 for 6 delay positions
	
	// Output-only meter parameters for UI display
	kInputMeterL = 1,
	kInputMeterR = 2,
	kOutputMeterL = 3,
	kOutputMeterR = 4,

	// Tap tempo (taptempo.h): hidden, set by tapping the logo, saved with the session.
	kTapOnParam = 5,      // 1 = the delay follows the tapped time
	kTapMsParam = 6,      // the tapped time, kTapMinMs..kTapMaxMs

	kParamCount = 7
};

// Button control tags (100-105 for the 6 delay buttons)
enum ButtonTags
{
	kDelayButton0 = 100,
	kDelayButton1 = 101,
	kDelayButton2 = 102,
	kDelayButton3 = 103,
	kDelayButton4 = 104,
	kDelayButton5 = 105
};

constexpr double kTapMinMs = 100.0;
constexpr double kTapMaxMs = 2000.0;
inline double tapMsOf(double norm) { return kTapMinMs + norm * (kTapMaxMs - kTapMinMs); }
inline double tapNormOf(double ms) { return (ms - kTapMinMs) / (kTapMaxMs - kTapMinMs); }

// Saved state after the delay index: this marker, then tap on (int32) and tap ms (double).
// A stream without it (every release before tap tempo) loads with tap tempo off.
constexpr Steinberg::int32 kTapStateMarker = 0x31504154;   // "TAP1"

//------------------------------------------------------------------------
} // namespace Yonie

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

// Message ID for meter data communication between processor and controller
inline const char* kMeterDataMessage = "MeterData";

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
	
	kParamCount = 5
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

//------------------------------------------------------------------------
} // namespace Yonie

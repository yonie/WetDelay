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
	kParamCount
};

//------------------------------------------------------------------------
} // namespace Yonie

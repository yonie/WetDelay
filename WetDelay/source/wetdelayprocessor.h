//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "delaybuffer.h"
#include "wetdelaycids.h"
#include <atomic>

namespace Yonie {

//------------------------------------------------------------------------
//  WetDelayProcessorProcessor
//------------------------------------------------------------------------
class WetDelayProcessorProcessor : public Steinberg::Vst::AudioEffect
{
public:
	WetDelayProcessorProcessor ();
	~WetDelayProcessorProcessor () SMTG_OVERRIDE;

    // Create function
	static Steinberg::FUnknown* createInstance (void* /*context*/) 
	{ 
		return (Steinberg::Vst::IAudioProcessor*)new WetDelayProcessorProcessor; 
	}

	//--- ---------------------------------------------------------------------
	// AudioEffect overrides:
	//--- ---------------------------------------------------------------------
	/** Called at first after constructor */
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	
	/** Called at the end before destructor */
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;
	
	/** Switch the Plug-in on/off */
	Steinberg::tresult PLUGIN_API setActive (Steinberg::TBool state) SMTG_OVERRIDE;

	/** Will be called before any process call */
	Steinberg::tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& newSetup) SMTG_OVERRIDE;
	
	/** Asks if a given sample size is supported see SymbolicSampleSizes. */
	Steinberg::tresult PLUGIN_API canProcessSampleSize (Steinberg::int32 symbolicSampleSize) SMTG_OVERRIDE;

	/** Here we go...the process call */
	Steinberg::tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;
		
	/** For persistence */
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;

//------------------------------------------------------------------------
protected:
	// Delay buffer
	DelayBuffer delayBuffer;
	
	// Current delay time index (0-5)
	int currentDelayIndex = 0;
	
	// Delay time values in milliseconds
	static const int DELAY_TIMES_MS[6];
	
	// Peak level meters (thread-safe)
	std::atomic<float> inputPeakL{0.0f};
	std::atomic<float> inputPeakR{0.0f};
	std::atomic<float> outputPeakL{0.0f};
	std::atomic<float> outputPeakR{0.0f};
	
	// Meter decay factor
	static constexpr float METER_DECAY = 0.9995f;
	
	// Meter update timing
	Steinberg::int32 samplesSinceLastMeterUpdate = 0;
	Steinberg::int32 meterUpdateInterval = 2205;  // ~50ms at 44100Hz
	
	// Update peak meter
	void updatePeak(float sample, std::atomic<float>& peak);
	
	// Send meter data to controller via message
	void sendMeterData();
};

//------------------------------------------------------------------------
} // namespace Yonie

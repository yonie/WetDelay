//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "wetdelayprocessor.h"
#include "wetdelaycids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include <cmath>

using namespace Steinberg;

namespace Yonie {

//------------------------------------------------------------------------
// Delay times in milliseconds
const int WetDelayProcessorProcessor::DELAY_TIMES_MS[6] = {
	20, 40, 80, 120, 220, 400
};
//------------------------------------------------------------------------
// WetDelayProcessorProcessor
//------------------------------------------------------------------------
WetDelayProcessorProcessor::WetDelayProcessorProcessor ()
{
	//--- set the wanted controller for our processor
	setControllerClass (kWetDelayProcessorControllerUID);
}

//------------------------------------------------------------------------
WetDelayProcessorProcessor::~WetDelayProcessorProcessor ()
{}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorProcessor::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated
	
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	// if everything Ok, continue
	if (result != kResultOk)
	{
		return result;
	}

	//--- create Audio IO ------
	addAudioInput (STR16 ("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

	/* If you don't need an event bus, you can remove the next line */
	addEventInput (STR16 ("Event In"), 1);

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorProcessor::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!
	
	//---do not forget to call parent ------
	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorProcessor::setActive (TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
	if (state)
	{
		// Reset meters when activated
		inputPeakL = 0.0f;
		inputPeakR = 0.0f;
		outputPeakL = 0.0f;
		outputPeakR = 0.0f;
	}
	return AudioEffect::setActive (state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorProcessor::process (Vst::ProcessData& data)
{
	//--- Read parameter changes -----------
	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			if (auto* paramQueue = data.inputParameterChanges->getParameterData (index))
			{
				Vst::ParamValue value;
				int32 sampleOffset;
				int32 numPoints = paramQueue->getPointCount ();
				
				if (paramQueue->getParameterId () == kDelayTimeParam && numPoints > 0)
				{
					// Get last parameter change
					if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) == kResultTrue)
					{
						// Convert normalized value (0.0-1.0) to index (0-5)
						currentDelayIndex = static_cast<int>(value * 5.0 + 0.5);
						if (currentDelayIndex < 0) currentDelayIndex = 0;
						if (currentDelayIndex > 5) currentDelayIndex = 5;
					}
				}
			}
		}
	}
	
	//--- Process audio -----------
	if (data.numInputs == 0 || data.numOutputs == 0)
		return kResultOk;
		
	if (data.numSamples > 0)
	{
		// Get input and output buffers
		Vst::AudioBusBuffers& input = data.inputs[0];
		Vst::AudioBusBuffers& output = data.outputs[0];
		
		// Ensure we have stereo
		if (input.numChannels >= 2 && output.numChannels >= 2)
		{
			float* inputL = input.channelBuffers32[0];
			float* inputR = input.channelBuffers32[1];
			float* outputL = output.channelBuffers32[0];
			float* outputR = output.channelBuffers32[1];
			
			// Measure input levels
			for (int32 i = 0; i < data.numSamples; i++)
			{
				updatePeak(inputL[i], inputPeakL);
				updatePeak(inputR[i], inputPeakR);
			}
			
			// Process delay (100% wet)
			int delayMs = DELAY_TIMES_MS[currentDelayIndex];
			delayBuffer.processStereo(inputL, outputL, inputR, outputR,
			                          data.numSamples, delayMs);
			
			// Measure output levels
			for (int32 i = 0; i < data.numSamples; i++)
			{
				updatePeak(outputL[i], outputPeakL);
				updatePeak(outputR[i], outputPeakR);
			}
			
			// Output is not silent
			output.silenceFlags = 0;
		}
		else
		{
			// Clear output if not stereo
			for (int32 c = 0; c < output.numChannels; c++)
			{
				memset(output.channelBuffers32[c], 0,
				       data.numSamples * sizeof(Vst::Sample32));
			}
			output.silenceFlags = ((uint64)1 << output.numChannels) - 1;
		}
	}

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	//--- called before any processing ----
	// Initialize delay buffer with max delay time
	delayBuffer.prepare(newSetup.sampleRate, 400);  // 400ms max
	
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorProcessor::setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);
	
	int32 savedDelayIndex = 0;
	if (streamer.readInt32(savedDelayIndex) == kResultTrue)
	{
		currentDelayIndex = savedDelayIndex;
	}
	
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorProcessor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);
	
	streamer.writeInt32(currentDelayIndex);

	return kResultOk;
}

//------------------------------------------------------------------------
void WetDelayProcessorProcessor::updatePeak(float sample, std::atomic<float>& peak)
{
	float absSample = std::abs(sample);
	float currentPeak = peak.load();
	
	if (absSample > currentPeak)
	{
		// Attack: instant
		peak.store(absSample);
	}
	else
	{
		// Decay: exponential
		peak.store(currentPeak * METER_DECAY);
	}
}

//------------------------------------------------------------------------
} // namespace Yonie

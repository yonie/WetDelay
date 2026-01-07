//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "wetdelaycontroller.h"
#include "wetdelaycids.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "base/source/fstreamer.h"

using namespace Steinberg;

namespace Yonie {

//------------------------------------------------------------------------
// WetDelayProcessorController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorController::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk)
	{
		return result;
	}

	// Register delay time parameter (0-5 for 6 positions)
	Vst::StringListParameter* delayParam = new Vst::StringListParameter(
		STR16("Delay Time"),
		kDelayTimeParam,
		nullptr,
		Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsList
	);
	
	delayParam->appendString(STR16("20 ms"));
	delayParam->appendString(STR16("40 ms"));
	delayParam->appendString(STR16("80 ms"));
	delayParam->appendString(STR16("120 ms"));
	delayParam->appendString(STR16("220 ms"));
	delayParam->appendString(STR16("400 ms"));
	
	parameters.addParameter(delayParam);

	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorController::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorController::setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return kResultFalse;

	IBStreamer streamer(state, kLittleEndian);
	
	int32 savedDelayIndex = 0;
	if (streamer.readInt32(savedDelayIndex) == kResultTrue)
	{
		// Convert index to normalized value and set parameter
		setParamNormalized(kDelayTimeParam, savedDelayIndex / 5.0);
	}

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorController::setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API WetDelayProcessorController::getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API WetDelayProcessorController::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
		// create your editor here and return a IPlugView ptr of it
		auto* view = new VSTGUI::VST3Editor (this, "view", "wetdelayeditor.uidesc");
		return view;
	}
	return nullptr;
}

//------------------------------------------------------------------------
} // namespace Yonie

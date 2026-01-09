//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "wetdelaycontroller.h"
#include "wetdelaycids.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "base/source/fstreamer.h"
#include "customviewcreator.h"

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
	
	// Register read-only meter parameters for UI display
	parameters.addParameter(STR16("Input Meter L"), nullptr, 0, 0,
		Vst::ParameterInfo::kIsReadOnly, kInputMeterL);
	parameters.addParameter(STR16("Input Meter R"), nullptr, 0, 0,
		Vst::ParameterInfo::kIsReadOnly, kInputMeterR);
	parameters.addParameter(STR16("Output Meter L"), nullptr, 0, 0,
		Vst::ParameterInfo::kIsReadOnly, kOutputMeterL);
	parameters.addParameter(STR16("Output Meter R"), nullptr, 0, 0,
		Vst::ParameterInfo::kIsReadOnly, kOutputMeterR);

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
		currentDelayIndex = savedDelayIndex;
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
tresult PLUGIN_API WetDelayProcessorController::notify (Vst::IMessage* message)
{
	if (!message)
		return kInvalidArgument;
	
	// Check if this is our meter data message
	if (strcmp(message->getMessageID(), kMeterDataMessage) == 0)
	{
		const void* data = nullptr;
		uint32 size = 0;
		
		if (message->getAttributes()->getBinary("data", data, size) == kResultOk)
		{
			if (size == 4 * sizeof(float))
			{
				const float* meterData = static_cast<const float*>(data);
				
				// Update meter parameters - this will automatically update the UI
				setParamNormalized(kInputMeterL, meterData[0]);
				setParamNormalized(kInputMeterR, meterData[1]);
				setParamNormalized(kOutputMeterL, meterData[2]);
				setParamNormalized(kOutputMeterR, meterData[3]);
				
				return kResultOk;
			}
		}
	}
	
	return EditControllerEx1::notify(message);
}

//------------------------------------------------------------------------
void WetDelayProcessorController::setDelayIndexFromUI(int index)
{
	if (index >= 0 && index <= 5)
	{
		currentDelayIndex = index;
		// Convert to normalized value and update parameter
		Vst::ParamValue normalizedValue = index / 5.0;
		setParamNormalized(kDelayTimeParam, normalizedValue);
		
		// Notify host of parameter change
		if (componentHandler)
		{
			componentHandler->beginEdit(kDelayTimeParam);
			componentHandler->performEdit(kDelayTimeParam, normalizedValue);
			componentHandler->endEdit(kDelayTimeParam);
		}
	}
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
// DelayButtonController Implementation (simplified - may not be needed with CSegmentButton)
//------------------------------------------------------------------------
DelayButtonController::DelayButtonController(VSTGUI::IController* parentController,
                                              WetDelayProcessorController* mainCtrl)
    : DelegationController(parentController)
    , mainController(mainCtrl)
{
}

//------------------------------------------------------------------------
VSTGUI::CView* DelayButtonController::createView(const VSTGUI::UIAttributes& attributes,
                                                  const VSTGUI::IUIDescription* description)
{
    // Let parent create the view
    return DelegationController::createView(attributes, description);
}

//------------------------------------------------------------------------
VSTGUI::CView* DelayButtonController::verifyView(VSTGUI::CView* view,
                                                  const VSTGUI::UIAttributes& attributes,
                                                  const VSTGUI::IUIDescription* description)
{
    return DelegationController::verifyView(view, attributes, description);
}

//------------------------------------------------------------------------
void DelayButtonController::valueChanged(VSTGUI::CControl* control)
{
    // Forward to parent controller
    DelegationController::valueChanged(control);
}

//------------------------------------------------------------------------
void DelayButtonController::updateLEDIndicators(int selectedIndex)
{
    // Not needed with CSegmentButton - it handles visual state automatically
}

//------------------------------------------------------------------------
} // namespace Yonie

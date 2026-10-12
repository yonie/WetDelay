//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "wetdelaycontroller.h"
#include "buttonselectionframe.h"

#include <cmath>
#include <cstring>
#include "weteditor.h"
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

	registerCustomViews();

	// Register delay time parameter (0-5 for 6 positions)
	Vst::StringListParameter* delayParam = new Vst::StringListParameter(
		STR16("Delay Time"),
		kDelayTimeParam,
		nullptr,
		Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsList
	);
	
	delayParam->appendString(STR16("20ms"));
	delayParam->appendString(STR16("40ms"));
	delayParam->appendString(STR16("80ms"));
	delayParam->appendString(STR16("120ms"));
	delayParam->appendString(STR16("220ms"));
	delayParam->appendString(STR16("400ms"));
	
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

	// Tap tempo (taptempo.h). Hidden: set by tapping the logo, never offered for automation.
	parameters.addParameter(STR16("Tap Tempo"), nullptr, 1, 0,
		Vst::ParameterInfo::kIsHidden, kTapOnParam);
	parameters.addParameter(new Vst::RangeParameter(STR16("Tap Time"), kTapMsParam, STR16("ms"),
		kTapMinMs, kTapMaxMs, 500.0, static_cast<int32>(kTapMaxMs - kTapMinMs),
		Vst::ParameterInfo::kIsHidden));

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
	if (streamer.readInt32(savedDelayIndex))
	{
		currentDelayIndex = savedDelayIndex;
		// Convert index to normalized value and set parameter
		setParamNormalized(kDelayTimeParam, savedDelayIndex / 5.0);
	}

	// Tap tempo, appended later; a session saved before it ends here and loads with it off.
	int32 marker = 0, on = 0;
	double ms = 500.0;
	if (streamer.readInt32(marker) && marker == kTapStateMarker && streamer.readInt32(on) && streamer.readDouble(ms))
	{
		setParamNormalized(kTapOnParam, on ? 1.0 : 0.0);
		setParamNormalized(kTapMsParam, tapNormOf(ms));
	}
	else
		setParamNormalized(kTapOnParam, 0.0);

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
		auto* view = new Yonie::WetEditor (this, "view", "wetdelayeditor.uidesc");

		// Discrete zoom steps, in the editor's context menu under "UI Zoom".
		// VSTGUI handles the resize; the panel is a fixed layout, so free
		// dragging would buy nothing a step does not. 702x702 at 100% is small
		// on a high-DPI display, which is the actual complaint.
		//
		// Note that enabling zoom makes the editor open at a true 1:1 instead
		// of being scaled by the host, so on a high-DPI screen it now starts
		// smaller than it did. The menu is how that gets set.
		view->setAllowedZoomFactors({0.5, 0.75, 1.0, 1.25});
		// Opens at 75%: at 100% the panels are large on most screens. The zoom is not
		// saved, so every window opens here and nothing stored depends on it.
		view->setZoomFactor(0.75);
		return view;
	}
	return nullptr;
}

//------------------------------------------------------------------------
// Tap tempo
//------------------------------------------------------------------------
void WetDelayProcessorController::setHidden (Vst::ParamID id, double norm)
{
	setParamNormalized (id, norm);
	EditControllerEx1::beginEdit (id);
	performEdit (id, getParamNormalized (id));
	EditControllerEx1::endEdit (id);
}

// Taps more than two seconds apart start a new count. From the third tap the delay follows the
// average interval of the last five taps, and every further tap refines it.
void WetDelayProcessorController::tap ()
{
	const double now = tapClock ();
	if (!taps.empty () && now - taps.back () > 2.0)
		taps.clear ();
	taps.push_back (now);
	if (taps.size () > 5)
		taps.erase (taps.begin ());
	if (taps.size () < 3)
		return;
	double ms = 1000.0 * (taps.back () - taps.front ()) / (taps.size () - 1);
	ms = ms < kTapMinMs ? kTapMinMs : ms > kTapMaxMs ? kTapMaxMs : ms;
	beatOrigin = now;
	setHidden (kTapMsParam, tapNormOf (std::floor (ms + 0.5)));
	if (!tapActive ())
		setHidden (kTapOnParam, 1.0);
}

tresult WetDelayProcessorController::beginEdit (Vst::ParamID tag)
{
	if (tag == kDelayTimeParam && tapActive ())
	{
		taps.clear ();
		setHidden (kTapOnParam, 0.0);
	}
	return EditControllerEx1::beginEdit (tag);
}

VSTGUI::CView* WetDelayProcessorController::createCustomView (VSTGUI::UTF8StringPtr name,
	const VSTGUI::UIAttributes& attributes, const VSTGUI::IUIDescription*, VSTGUI::VST3Editor*)
{
	if (name && std::strcmp (name, "TapLogo") == 0)
	{
		VSTGUI::CPoint origin, size;
		attributes.getPointAttribute ("origin", origin);
		attributes.getPointAttribute ("size", size);
		return new TapLogo (VSTGUI::CRect (origin, size), this);
	}
	return nullptr;
}

VSTGUI::CView* WetDelayProcessorController::verifyView (VSTGUI::CView* view, const VSTGUI::UIAttributes&,
	const VSTGUI::IUIDescription*, VSTGUI::VST3Editor*)
{
	if (auto* frame = dynamic_cast<ButtonSelectionFrame*> (view))
		frame->setTapHost (this);
	return view;
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
    // No special handling needed for DelayTimeButtonGroup
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
    // Not needed with DelayTimeButtonGroup - ButtonSelectionFrame handles LEDs
}

//------------------------------------------------------------------------
} // namespace Yonie

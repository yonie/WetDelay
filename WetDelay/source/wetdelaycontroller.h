//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include "vstgui/uidescription/delegationcontroller.h"
#include "wetdelaycids.h"
#include "taptempo.h"

#include <vector>

namespace Yonie {

// Forward declarations
class DelayButtonController;

//------------------------------------------------------------------------
//  WetDelayProcessorController
//------------------------------------------------------------------------
class WetDelayProcessorController : public Steinberg::Vst::EditControllerEx1,
                                    public VSTGUI::VST3EditorDelegate,
                                    public TapHost
{
public:
//------------------------------------------------------------------------
	WetDelayProcessorController () = default;
	~WetDelayProcessorController () SMTG_OVERRIDE = default;

    // Create function
	static Steinberg::FUnknown* createInstance (void* /*context*/)
	{
		return (Steinberg::Vst::IEditController*)new WetDelayProcessorController;
	}

	//--- from IPluginBase -----------------------------------------------
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;

	//--- from EditController --------------------------------------------
	Steinberg::tresult PLUGIN_API setComponentState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::IPlugView* PLUGIN_API createView (Steinberg::FIDString name) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	
	// Get current delay index for UI updates
	// A press on any time button begins an edit of the delay time: it leaves tap tempo.
	Steinberg::tresult beginEdit (Steinberg::Vst::ParamID tag) SMTG_OVERRIDE;

	// VST3EditorDelegate: the logo's tap area, and the LEDs learn where the beat is.
	VSTGUI::CView* createCustomView (VSTGUI::UTF8StringPtr name, const VSTGUI::UIAttributes& attributes,
	                                 const VSTGUI::IUIDescription* description, VSTGUI::VST3Editor* editor) SMTG_OVERRIDE;
	VSTGUI::CView* verifyView (VSTGUI::CView* view, const VSTGUI::UIAttributes& attributes,
	                           const VSTGUI::IUIDescription* description, VSTGUI::VST3Editor* editor) SMTG_OVERRIDE;

	// TapHost
	void tap () override;
	bool tapActive () const override { return const_cast<WetDelayProcessorController*> (this)->getParamNormalized (kTapOnParam) >= 0.5; }
	double tapPeriodSeconds () const override { return tapMsOf (const_cast<WetDelayProcessorController*> (this)->getParamNormalized (kTapMsParam)) / 1000.0; }
	double tapOrigin () const override { return beatOrigin; }

	int getCurrentDelayIndex() const { return currentDelayIndex; }
	void setDelayIndexFromUI(int index);

	//---Interface---------
	DEFINE_INTERFACES
		// Here you can add more supported VST3 interfaces
		// DEF_INTERFACE (Vst::IXXX)
	END_DEFINE_INTERFACES (EditController)
    DELEGATE_REFCOUNT (EditController)

//------------------------------------------------------------------------
protected:
	int currentDelayIndex = 0;
	std::vector<double> taps;       // recent tap times, tapClock() seconds
	double beatOrigin = 0.0;
	void setHidden (Steinberg::Vst::ParamID id, double norm);
	friend class DelayButtonController;
};

//------------------------------------------------------------------------
// DelayButtonController - Sub-controller for delay button radio behavior
// Note: DelegationController already inherits from IControlListener
//------------------------------------------------------------------------
class DelayButtonController : public VSTGUI::DelegationController
{
public:
    DelayButtonController(VSTGUI::IController* parentController, 
                          WetDelayProcessorController* mainController);
    ~DelayButtonController() override = default;
    
    // IController overrides
    VSTGUI::CView* createView(const VSTGUI::UIAttributes& attributes, 
                              const VSTGUI::IUIDescription* description) override;
    VSTGUI::CView* verifyView(VSTGUI::CView* view, 
                              const VSTGUI::UIAttributes& attributes,
                              const VSTGUI::IUIDescription* description) override;
    
    // IControlListener (inherited from DelegationController)
    void valueChanged(VSTGUI::CControl* control) override;
    
    // Update LED indicators
    void updateLEDIndicators(int selectedIndex);
    
protected:
    WetDelayProcessorController* mainController = nullptr;
};

//------------------------------------------------------------------------
} // namespace Yonie

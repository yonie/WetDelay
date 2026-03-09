//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#pragma once

#include "vstgui/lib/cview.h"
#include "vstgui/lib/cviewcontainer.h"
#include "vstgui/lib/controls/ccontrol.h"

namespace Yonie {

//------------------------------------------------------------------------
// DelayTimeButtonGroup - Invisible radio button group for delay time selection
// Handles 6 clickable regions, enforces mutual exclusivity
// Draws nothing - visual feedback via ButtonSelectionFrame LEDs
//------------------------------------------------------------------------
class DelayTimeButtonGroup : public VSTGUI::CControl
{
public:
	DelayTimeButtonGroup(const VSTGUI::CRect& size);
	~DelayTimeButtonGroup() override = default;

	// CView overrides
	void draw(VSTGUI::CDrawContext* context) override;
	VSTGUI::CMouseEventResult onMouseDown(VSTGUI::CPoint& where, const VSTGUI::CButtonState& buttons) override;
	
	// CBaseObject override (required by CControl)
	VSTGUI::CBaseObject* newCopy() const override { return new DelayTimeButtonGroup(*this); }
	
	// Get current selection (0-5)
	int getSelection() const { return currentSelection; }
	
	// Set selection programmatically
	void setSelection(int index);

protected:
	// Hit test: returns button index 0-5, or -1 if miss
	int hitTest(const VSTGUI::CPoint& where) const;

private:
	int currentSelection = 0;
	
	// Button geometry (same as ButtonSelectionFrame)
	static constexpr VSTGUI::CCoord kButtonOffsets[6] = { 1, 110, 218, 327, 435, 543 };
	static constexpr VSTGUI::CCoord kButtonWidth = 87;
	static constexpr VSTGUI::CCoord kButtonHeight = 119;
};

//------------------------------------------------------------------------
} // namespace Yonie
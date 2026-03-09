//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "delaytimebuttonsgroup.h"

using namespace VSTGUI;

namespace Yonie {

//------------------------------------------------------------------------
DelayTimeButtonGroup::DelayTimeButtonGroup(const CRect& size)
    : CControl(size, nullptr, -1)
{
    setMouseEnabled(true);
    setWantsFocus(false);
}

//------------------------------------------------------------------------
void DelayTimeButtonGroup::draw(CDrawContext* context)
{
    // Draw nothing - backplate shows through with its prettier labels
    setDirty(false);
}

//------------------------------------------------------------------------
CMouseEventResult DelayTimeButtonGroup::onMouseDown(CPoint& where, const CButtonState& buttons)
{
    // Convert to local coordinates
    CPoint localPoint = where;
    localPoint.offset(-getViewSize().left, -getViewSize().top);
    
    int clickedButton = hitTest(localPoint);
    
    if (clickedButton >= 0)  // Click inside a button region
    {
        if (clickedButton != currentSelection)
        {
            currentSelection = clickedButton;
            
            // Update the control value (0-5 mapped to normalized 0.0-1.0)
            setValueNormalized(static_cast<float>(currentSelection) / 5.0f);
            
            // Begin/end edit for proper automation support
            beginEdit();
            valueChanged();
            endEdit();
            
            invalid();
        }
        return kMouseEventHandled;  // We consumed this click
    }
    
    // Click missed all buttons - allow propagation to views below
    return kMouseEventNotHandled;
}

//------------------------------------------------------------------------
void DelayTimeButtonGroup::setSelection(int index)
{
    if (index >= 0 && index <= 5 && index != currentSelection)
    {
        currentSelection = index;
        setValueNormalized(static_cast<float>(currentSelection) / 5.0f);
        invalid();
    }
}

//------------------------------------------------------------------------
int DelayTimeButtonGroup::hitTest(const CPoint& where) const
{
    for (int i = 0; i < 6; i++)
    {
        CRect buttonRect(kButtonOffsets[i], 0, kButtonOffsets[i] + kButtonWidth, kButtonHeight);
        if (buttonRect.pointInside(where))
            return i;
    }
    return -1;
}

//------------------------------------------------------------------------
} // namespace Yonie
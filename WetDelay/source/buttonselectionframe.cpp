//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "buttonselectionframe.h"

using namespace VSTGUI;

namespace Yonie {

// Hardcoded button X positions (relative to view left at x=39)
// Absolute positions: 40, 149, 257, 366, 474, 582
// Each button is 87px wide, 119px tall
static const CCoord kButtonOffsets[6] = { 1, 110, 218, 327, 435, 543 };
static const CCoord kButtonWidth = 87;
static const int kNumButtons = 6;

//------------------------------------------------------------------------
// ButtonSelectionFrame Implementation
//------------------------------------------------------------------------
ButtonSelectionFrame::ButtonSelectionFrame(const CRect& size)
    : CControl(size, nullptr, -1)
{
    setMouseEnabled(false);  // Click through to button beneath
    setWantsFocus(false);
}

//------------------------------------------------------------------------
void ButtonSelectionFrame::draw(CDrawContext* context)
{
    CRect viewSize = getViewSize();
    
    // Calculate which button is selected (0 to 5)
    float val = getValueNormalized();
    int selectedIndex = static_cast<int>(val * 5 + 0.5f);
    if (selectedIndex < 0) selectedIndex = 0;
    if (selectedIndex > 5) selectedIndex = 5;
    
    // Draw LED indicator for each button
    for (int i = 0; i < kNumButtons; i++)
    {
        // Calculate LED position - centered at top of each button
        CCoord buttonX = viewSize.left + kButtonOffsets[i];
        CCoord buttonCenterX = buttonX + kButtonWidth / 2;
        
        // LED positioned at top of button area with small margin
        CCoord ledX = buttonCenterX - ledWidth / 2;
        CCoord ledY = viewSize.top + 12;  // 12px from top
        
        CRect ledRect(ledX, ledY, ledX + ledWidth, ledY + ledHeight);
        
        // Choose color based on selection state
        bool isSelected = (i == selectedIndex);
        
        if (isSelected)
        {
            // Draw glow effect behind the LED for "on" state
            CRect glowRect = ledRect;
            glowRect.extend(2, 1);
            CColor glowColor = ledOnColor;
            glowColor.alpha = 60;
            context->setFillColor(glowColor);
            context->drawRect(glowRect, kDrawFilled);
        }
        
        // Draw the LED bar
        CColor color = isSelected ? ledOnColor : ledOffColor;
        context->setFillColor(color);
        context->drawRect(ledRect, kDrawFilled);
    }
    
    setDirty(false);
}

//------------------------------------------------------------------------
void ButtonSelectionFrame::setValueNormalized(float val)
{
    if (val != getValueNormalized())
    {
        CControl::setValueNormalized(val);
        invalid();  // Request redraw
    }
}

//------------------------------------------------------------------------
} // namespace Yonie
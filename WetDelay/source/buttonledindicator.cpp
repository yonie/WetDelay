//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "buttonledindicator.h"

using namespace VSTGUI;

namespace Yonie {

//------------------------------------------------------------------------
// ButtonLEDIndicator Implementation
//------------------------------------------------------------------------
ButtonLEDIndicator::ButtonLEDIndicator(const CRect& size)
    : CView(size)
{
    setMouseEnabled(false);  // Click through to button beneath
}

//------------------------------------------------------------------------
void ButtonLEDIndicator::draw(CDrawContext* context)
{
    CRect rect = getViewSize();
    
    // Only draw when active - red outline/frame around the button
    if (active)
    {
        // Draw red rectangle outline (frame only, no fill)
        context->setFrameColor(activeColor);
        context->setLineWidth(3);
        
        // Inset slightly to be visible within button bounds
        CRect frameRect = rect;
        frameRect.inset(2, 2);
        context->drawRect(frameRect, kDrawStroked);
    }
    // When inactive, draw nothing (transparent)
    
    setDirty(false);
}

//------------------------------------------------------------------------
void ButtonLEDIndicator::setActive(bool newActive)
{
    if (active != newActive)
    {
        active = newActive;
        invalid();  // Request redraw
    }
}

//------------------------------------------------------------------------
} // namespace Yonie
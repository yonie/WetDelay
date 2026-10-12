//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "ledmeterview.h"
#include "meterlevel.h"

#include <cmath>

using namespace VSTGUI;

namespace Yonie {

//------------------------------------------------------------------------
// LEDMeterView Implementation
//------------------------------------------------------------------------
LEDMeterView::LEDMeterView(const CRect& size)
    : CControl(size, nullptr, -1)
{
    setWantsFocus(false);
}

//------------------------------------------------------------------------
int LEDMeterView::litSegments() const
{
    const float level = getValueNormalized();

    // A level meter: the line's one scale, only the block count is ours (meterlevel.h).
    return Meter::litSegments(level, numSegments);
}

//------------------------------------------------------------------------
void LEDMeterView::draw(CDrawContext* context)
{
    const int litCount = litSegments();
    
    // Draw each segment
    for (int i = 0; i < numSegments; i++)
    {
        CRect segRect = calculateSegmentRect(i);
        CColor color = (i < litCount) ? getLitColor(i) : getDarkColor(i);
        
        context->setFillColor(color);
        context->drawRect(segRect, kDrawFilled);
    }
    
    setDirty(false);
}

//------------------------------------------------------------------------
CRect LEDMeterView::calculateSegmentRect(int segmentIndex) const
{
    CRect viewSize = getViewSize();
    CCoord totalWidth = viewSize.getWidth();
    CCoord totalHeight = viewSize.getHeight();
    
    if (isHorizontal)
    {
        // Horizontal meter: segments arranged left to right
        CCoord segmentWidth = (totalWidth - (numSegments - 1) * segmentGap) / numSegments;
        CCoord x = viewSize.left + segmentIndex * (segmentWidth + segmentGap);
        
        return CRect(x, viewSize.top, x + segmentWidth, viewSize.bottom);
    }
    else
    {
        // Vertical meter: segments arranged bottom to top
        CCoord segmentHeight = (totalHeight - (numSegments - 1) * segmentGap) / numSegments;
        CCoord y = viewSize.bottom - (segmentIndex + 1) * segmentHeight - segmentIndex * segmentGap;
        
        return CRect(viewSize.left, y, viewSize.right, y + segmentHeight);
    }
}

//------------------------------------------------------------------------
CColor LEDMeterView::getLitColor(int segmentIndex) const
{
    switch (Meter::segmentZone(segmentIndex, numSegments))
    {
        case Meter::kGreen:  return greenLit;
        case Meter::kYellow: return yellowLit;
        default:             return redLit;
    }
}

//------------------------------------------------------------------------
CColor LEDMeterView::getDarkColor(int segmentIndex) const
{
    switch (Meter::segmentZone(segmentIndex, numSegments))
    {
        case Meter::kGreen:  return greenDark;
        case Meter::kYellow: return yellowDark;
        default:             return redDark;
    }
}

//------------------------------------------------------------------------
} // namespace Yonie
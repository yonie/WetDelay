//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "ledmeterview.h"

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
void LEDMeterView::draw(CDrawContext* context)
{
    // Get normalized value (0.0 to 1.0)
    float level = getValueNormalized();
    
    // Calculate how many segments should be lit
    int litCount = static_cast<int>(level * numSegments + 0.5f);
    if (litCount > numSegments) litCount = numSegments;
    if (litCount < 0) litCount = 0;
    
    // Draw each segment
    for (int i = 0; i < numSegments; i++)
    {
        CRect segRect = calculateSegmentRect(i);
        CColor color = (i < litCount) ? getLitColor(i) : getDarkColor(i);
        
        context->setFillColor(color);
        context->drawRect(segRect, kDrawFilled);
    }
    
    // Draw frame around entire meter (optional, subtle)
    // context->setFrameColor(CColor(60, 60, 60, 255));
    // context->setLineWidth(1);
    // context->drawRect(getViewSize(), kDrawStroked);
    
    setDirty(false);
}

//------------------------------------------------------------------------
void LEDMeterView::setValueNormalized(float val)
{
    if (val != getValueNormalized())
    {
        CControl::setValueNormalized(val);
        invalid();  // Request redraw
    }
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
    // 12 segments: 0-7 green, 8-9 yellow, 10-11 red
    // Scale for different segment counts
    float position = static_cast<float>(segmentIndex) / (numSegments - 1);
    
    if (position < 0.67f)  // First ~67% (green zone)
    {
        return greenLit;
    }
    else if (position < 0.83f)  // Next ~16% (yellow zone)
    {
        return yellowLit;
    }
    else  // Last ~17% (red zone)
    {
        return redLit;
    }
}

//------------------------------------------------------------------------
CColor LEDMeterView::getDarkColor(int segmentIndex) const
{
    // Same zones as lit colors, but darker
    float position = static_cast<float>(segmentIndex) / (numSegments - 1);
    
    if (position < 0.67f)
    {
        return greenDark;
    }
    else if (position < 0.83f)
    {
        return yellowDark;
    }
    else
    {
        return redDark;
    }
}

//------------------------------------------------------------------------
} // namespace Yonie
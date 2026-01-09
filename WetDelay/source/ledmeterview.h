//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#pragma once

#include "vstgui/lib/cview.h"
#include "vstgui/lib/controls/ccontrol.h"
#include "vstgui/lib/cdrawcontext.h"

namespace Yonie {

//------------------------------------------------------------------------
// LEDMeterView - Custom 80's style LED segment meter
// Displays 12 discrete LED segments: 8 green, 2 yellow, 2 red
//------------------------------------------------------------------------
class LEDMeterView : public VSTGUI::CControl
{
public:
    LEDMeterView(const VSTGUI::CRect& size);
    ~LEDMeterView() override = default;

    // CView overrides
    void draw(VSTGUI::CDrawContext* context) override;

    // CControl overrides
    void setValueNormalized(float val) override;
    
    // Configuration
    void setNumSegments(int num) { numSegments = num; }
    void setSegmentGap(VSTGUI::CCoord gap) { segmentGap = gap; }
    void setHorizontal(bool horiz) { isHorizontal = horiz; }
    
    // Custom view class name for VSTGUI factory
    CLASS_METHODS(LEDMeterView, CControl)

protected:
    VSTGUI::CRect calculateSegmentRect(int segmentIndex) const;
    VSTGUI::CColor getLitColor(int segmentIndex) const;
    VSTGUI::CColor getDarkColor(int segmentIndex) const;
    
    int numSegments = 12;
    VSTGUI::CCoord segmentGap = 2;
    bool isHorizontal = true;
    
    // Color definitions for 80's LED look
    // Green zone (segments 0-7)
    VSTGUI::CColor greenLit{0, 255, 0, 255};
    VSTGUI::CColor greenDark{10, 42, 10, 255};
    
    // Yellow zone (segments 8-9)
    VSTGUI::CColor yellowLit{255, 255, 0, 255};
    VSTGUI::CColor yellowDark{42, 42, 10, 255};
    
    // Red zone (segments 10-11)
    VSTGUI::CColor redLit{255, 0, 0, 255};
    VSTGUI::CColor redDark{42, 10, 10, 255};
};

//------------------------------------------------------------------------
} // namespace Yonie
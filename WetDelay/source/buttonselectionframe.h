//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#pragma once

#include "vstgui/lib/controls/ccontrol.h"
#include "vstgui/lib/cdrawcontext.h"

namespace Yonie {

//------------------------------------------------------------------------
// ButtonSelectionFrame - Draws 80's style LED indicators for delay buttons
// Shows bright red LED for selected button, dark red for others
//------------------------------------------------------------------------
class ButtonSelectionFrame : public VSTGUI::CControl
{
public:
    ButtonSelectionFrame(const VSTGUI::CRect& size);
    ~ButtonSelectionFrame() override = default;

    // CView overrides
    void draw(VSTGUI::CDrawContext* context) override;

    // CControl overrides
    void setValueNormalized(float val) override;
    
    // Configuration
    void setLedOnColor(const VSTGUI::CColor& color) { ledOnColor = color; }
    void setLedOffColor(const VSTGUI::CColor& color) { ledOffColor = color; }
    void setLedWidth(VSTGUI::CCoord width) { ledWidth = width; }
    void setLedHeight(VSTGUI::CCoord height) { ledHeight = height; }
    
    // Legacy support - maps to LED colors
    void setFrameColor(const VSTGUI::CColor& color) { ledOnColor = color; }
    void setFrameWidth(VSTGUI::CCoord width) { (void)width; }  // Ignored
    
    CLASS_METHODS(ButtonSelectionFrame, CControl)

protected:
    // 80's style LED colors (matching LEDMeterView red zone)
    VSTGUI::CColor ledOnColor{255, 0, 0, 255};    // Bright red when selected
    VSTGUI::CColor ledOffColor{42, 10, 10, 255};  // Dark red when not selected
    
    // LED dimensions
    VSTGUI::CCoord ledWidth = 50;   // Width of LED bar
    VSTGUI::CCoord ledHeight = 6;   // Height of LED bar
};

//------------------------------------------------------------------------
} // namespace Yonie
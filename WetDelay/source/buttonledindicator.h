//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#pragma once

#include "vstgui/lib/cview.h"
#include "vstgui/lib/cdrawcontext.h"

namespace Yonie {

//------------------------------------------------------------------------
// ButtonLEDIndicator - 80's style LED stripe indicator for delay buttons
// Shows bright red when active, dark maroon when inactive
//------------------------------------------------------------------------
class ButtonLEDIndicator : public VSTGUI::CView
{
public:
    ButtonLEDIndicator(const VSTGUI::CRect& size);
    ~ButtonLEDIndicator() override = default;

    // CView overrides
    void draw(VSTGUI::CDrawContext* context) override;
    
    // State control
    void setActive(bool active);
    bool isActive() const { return active; }
    
    // Configuration
    void setActiveColor(const VSTGUI::CColor& color) { activeColor = color; }
    void setInactiveColor(const VSTGUI::CColor& color) { inactiveColor = color; }
    
    // For linking to delay value (0-5)
    void setButtonIndex(int index) { buttonIndex = index; }
    int getButtonIndex() const { return buttonIndex; }

    CLASS_METHODS(ButtonLEDIndicator, CView)

protected:
    bool active = false;
    int buttonIndex = 0;
    
    // 80's style LED colors
    VSTGUI::CColor activeColor{255, 0, 0, 255};      // Bright red
    VSTGUI::CColor inactiveColor{58, 10, 10, 255};   // Dark maroon
};

//------------------------------------------------------------------------
} // namespace Yonie
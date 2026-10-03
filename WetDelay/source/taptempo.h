//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------
// Tap tempo, the hidden mode (Ronald, 2026-10-02). Tap the WET DELAY logo to the beat: from the
// third tap the delay follows the tapped time and all six LEDs blink on the beat. Pressing any
// of the six time buttons leaves the mode. Nothing on the panel changes.
//
// TapHost is what the views need from the controller; the controller holds the taps and the two
// hidden parameters (kTapOnParam, kTapMsParam) the processor reads.
//------------------------------------------------------------------------

#pragma once

#include "vstgui/lib/cview.h"

#include <chrono>

namespace Yonie {

struct TapHost
{
    virtual ~TapHost() = default;
    virtual void tap() = 0;                     // the logo was tapped, now
    virtual bool tapActive() const = 0;
    virtual double tapPeriodSeconds() const = 0;
    virtual double tapOrigin() const = 0;       // seconds on tapClock() of a beat

    static double tapClock()
    {
        using namespace std::chrono;
        return duration<double>(steady_clock::now().time_since_epoch()).count();
    }
};

// The click area over the painted logo. Draws nothing.
class TapLogo : public VSTGUI::CView
{
public:
    TapLogo(const VSTGUI::CRect& size, TapHost* host) : CView(size), host(host)
    {
        setTransparency(true);
        setWantsFocus(false);
    }

    void draw(VSTGUI::CDrawContext*) override { setDirty(false); }

    VSTGUI::CMouseEventResult onMouseDown(VSTGUI::CPoint&, const VSTGUI::CButtonState& buttons) override
    {
        if (!buttons.isLeftButton() || !host)
            return VSTGUI::kMouseEventNotHandled;
        host->tap();
        return VSTGUI::kMouseEventHandled;
    }

private:
    TapHost* host;
};

} // namespace Yonie

//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------
// The editor every WET plug-in opens. Canonical copy in wet-toolkit/tools; each plug-in
// carries its own copy in source/, like monobus.h.
//
// Two changes to VSTGUI's VST3Editor, and nothing else:
//
// 1. The zoom steps sit directly in the right-click menu ("UI Zoom 75%" ...), not in a
//    "UI Zoom" submenu. When a host offers its own context menu (Studio One does),
//    VSTGUI hands our entries to it, and a host may leave a plug-in's submenu out -
//    then there is no way to zoom at all (WetEQ #1).
//
// 2. canResize() says no. The panel is a fixed layout that changes size only through
//    those zoom steps, and VSTGUI's own answer is always yes, so hosts drew resize
//    arrows on the window edge that did nothing (WetDelay #2). A zoom step still
//    resizes the window: that goes through resizeView, which canResize does not gate.
//------------------------------------------------------------------------

#pragma once

#include "vstgui/plugin-bindings/vst3editor.h"
#include "vstgui/lib/controls/coptionmenu.h"
#include "vstgui/lib/events.h"

#include <cstdio>
#include <vector>

namespace Yonie {

class WetEditor : public VSTGUI::VST3Editor
{
public:
    using VSTGUI::VST3Editor::VST3Editor;

    Steinberg::tresult PLUGIN_API canResize() override { return Steinberg::kResultFalse; }

    void onMouseEvent(VSTGUI::MouseEvent& event, VSTGUI::CFrame* frame) override
    {
        if (event.type != VSTGUI::EventType::MouseDown || !event.buttonState.isRight() ||
            allowedZoomFactors.empty())
            return VSTGUI::VST3Editor::onMouseEvent(event, frame);

        // VST3Editor builds the menu from its delegate and appends the zoom submenu
        // itself. For this one click, stand a delegate in front of the real one that
        // adds the zoom steps as plain entries, and hide the factors so the submenu is
        // not built as well. The entries use VST3Editor's own "Zoom" command, which reads
        // allowedZoomFactors when chosen - after both are put back.
        const auto factors = allowedZoomFactors;
        ZoomEntries entries(delegate, factors, getZoomFactor());
        auto* realDelegate = delegate;
        delegate = &entries;
        allowedZoomFactors.clear();
        VSTGUI::VST3Editor::onMouseEvent(event, frame);
        delegate = realDelegate;
        allowedZoomFactors = factors;
    }

private:
    // Forwards what VST3Editor asks of its delegate during a right-click - the menu and
    // the parameter under the mouse - and adds the zoom entries to the menu.
    struct ZoomEntries : VSTGUI::VST3EditorDelegate
    {
        ZoomEntries(VSTGUI::IVST3EditorDelegate* real, const std::vector<double>& factors, double current)
        : real(real), factors(factors), current(current) {}

        VSTGUI::COptionMenu* createContextMenu(const VSTGUI::CPoint& pos, VSTGUI::VST3Editor* editor) override
        {
            auto* menu = real ? real->createContextMenu(pos, editor) : nullptr;
            if (!menu)
                menu = new VSTGUI::COptionMenu();
            else if (menu->getNbEntries() > 0)
                menu->addSeparator();
            for (size_t i = 0; i < factors.size(); ++i)
            {
                char title[32];
                std::snprintf(title, sizeof(title), "UI Zoom %d%%", static_cast<int>(factors[i] * 100.0 + 0.5));
                auto* item = menu->addEntry(new VSTGUI::CCommandMenuItem(
                    {title, static_cast<int32_t>(i), editor, "Zoom", title}));
                if (factors[i] == current)
                    item->setChecked(true);
            }
            return menu;
        }
        bool findParameter(const VSTGUI::CPoint& pos, Steinberg::Vst::ParamID& id, VSTGUI::VST3Editor* editor) override
        {
            return real && real->findParameter(pos, id, editor);
        }
        bool isPrivateParameter(const Steinberg::Vst::ParamID id) override
        {
            return real && real->isPrivateParameter(id);
        }

        VSTGUI::IVST3EditorDelegate* real;
        const std::vector<double>& factors;
        double current;
    };
};

} // namespace Yonie

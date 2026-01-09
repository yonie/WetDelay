//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#pragma once

#include "vstgui/uidescription/iviewcreator.h"
#include "vstgui/uidescription/uiviewfactory.h"
#include "vstgui/uidescription/uiattributes.h"
#include "vstgui/uidescription/iuidescription.h"

#include "ledmeterview.h"
#include "buttonledindicator.h"
#include "buttonselectionframe.h"

namespace Yonie {

//------------------------------------------------------------------------
// LEDMeterViewCreator - Factory for creating LEDMeterView
//------------------------------------------------------------------------
class LEDMeterViewCreator : public VSTGUI::ViewCreatorAdapter
{
public:
    LEDMeterViewCreator();
    
    // IViewCreator
    VSTGUI::IdStringPtr getViewName() const override { return "LEDMeterView"; }
    VSTGUI::IdStringPtr getBaseViewName() const override { return "CControl"; }
    VSTGUI::UTF8StringPtr getDisplayName() const override { return "LED Meter View"; }
    
    VSTGUI::CView* create(const VSTGUI::UIAttributes& attributes,
                          const VSTGUI::IUIDescription* description) const override;
    
    bool apply(VSTGUI::CView* view, const VSTGUI::UIAttributes& attributes,
               const VSTGUI::IUIDescription* description) const override;
    
    bool getAttributeNames(StringList& attributeNames) const override;
    AttrType getAttributeType(const string& attributeName) const override;
    bool getAttributeValue(VSTGUI::CView* view, const string& attributeName,
                           string& stringValue, const VSTGUI::IUIDescription* desc) const override;
};

//------------------------------------------------------------------------
// ButtonLEDIndicatorCreator - Factory for creating ButtonLEDIndicator
//------------------------------------------------------------------------
class ButtonLEDIndicatorCreator : public VSTGUI::ViewCreatorAdapter
{
public:
    ButtonLEDIndicatorCreator();
    
    // IViewCreator
    VSTGUI::IdStringPtr getViewName() const override { return "ButtonLEDIndicator"; }
    VSTGUI::IdStringPtr getBaseViewName() const override { return "CView"; }
    VSTGUI::UTF8StringPtr getDisplayName() const override { return "Button LED Indicator"; }
    
    VSTGUI::CView* create(const VSTGUI::UIAttributes& attributes,
                          const VSTGUI::IUIDescription* description) const override;
    
    bool apply(VSTGUI::CView* view, const VSTGUI::UIAttributes& attributes,
               const VSTGUI::IUIDescription* description) const override;
    
    bool getAttributeNames(StringList& attributeNames) const override;
    AttrType getAttributeType(const string& attributeName) const override;
    bool getAttributeValue(VSTGUI::CView* view, const string& attributeName,
                           string& stringValue, const VSTGUI::IUIDescription* desc) const override;
};

//------------------------------------------------------------------------
// ButtonSelectionFrameCreator - Factory for creating ButtonSelectionFrame
//------------------------------------------------------------------------
class ButtonSelectionFrameCreator : public VSTGUI::ViewCreatorAdapter
{
public:
    ButtonSelectionFrameCreator();
    
    // IViewCreator
    VSTGUI::IdStringPtr getViewName() const override { return "ButtonSelectionFrame"; }
    VSTGUI::IdStringPtr getBaseViewName() const override { return "CControl"; }
    VSTGUI::UTF8StringPtr getDisplayName() const override { return "Button Selection Frame"; }
    
    VSTGUI::CView* create(const VSTGUI::UIAttributes& attributes,
                          const VSTGUI::IUIDescription* description) const override;
    
    bool apply(VSTGUI::CView* view, const VSTGUI::UIAttributes& attributes,
               const VSTGUI::IUIDescription* description) const override;
    
    bool getAttributeNames(StringList& attributeNames) const override;
    AttrType getAttributeType(const string& attributeName) const override;
    bool getAttributeValue(VSTGUI::CView* view, const string& attributeName,
                           string& stringValue, const VSTGUI::IUIDescription* desc) const override;
};

//------------------------------------------------------------------------
// Register custom views - call this during plugin initialization
//------------------------------------------------------------------------
void registerCustomViews();

//------------------------------------------------------------------------
} // namespace Yonie
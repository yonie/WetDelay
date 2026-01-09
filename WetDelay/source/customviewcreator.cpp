//------------------------------------------------------------------------
// Copyright(c) 2026 Yonie.
//------------------------------------------------------------------------

#include "customviewcreator.h"
#include "vstgui/uidescription/uiviewcreator.h"

using namespace VSTGUI;

namespace Yonie {

// Attribute names
static const std::string kAttrNumSegments = "num-segments";
static const std::string kAttrSegmentGap = "segment-gap";
static const std::string kAttrHorizontal = "horizontal";
static const std::string kAttrButtonIndex = "button-index";
static const std::string kAttrActiveColor = "active-color";
static const std::string kAttrInactiveColor = "inactive-color";
static const std::string kAttrFrameColor = "frame-color";
static const std::string kAttrFrameWidth = "frame-width";

//------------------------------------------------------------------------
// LEDMeterViewCreator Implementation
//------------------------------------------------------------------------
LEDMeterViewCreator::LEDMeterViewCreator()
{
    UIViewFactory::registerViewCreator(*this);
}

//------------------------------------------------------------------------
CView* LEDMeterViewCreator::create(const UIAttributes& attributes, 
                                    const IUIDescription* description) const
{
    CRect size(0, 0, 200, 20);
    return new LEDMeterView(size);
}

//------------------------------------------------------------------------
bool LEDMeterViewCreator::apply(CView* view, const UIAttributes& attributes, 
                                 const IUIDescription* description) const
{
    auto* meterView = dynamic_cast<LEDMeterView*>(view);
    if (!meterView)
        return false;
    
    int32_t intValue;
    if (attributes.getIntegerAttribute(kAttrNumSegments, intValue))
        meterView->setNumSegments(intValue);
    
    double doubleValue;
    if (attributes.getDoubleAttribute(kAttrSegmentGap, doubleValue))
        meterView->setSegmentGap(static_cast<CCoord>(doubleValue));
    
    bool boolValue;
    if (attributes.getBooleanAttribute(kAttrHorizontal, boolValue))
        meterView->setHorizontal(boolValue);
    
    return true;
}

//------------------------------------------------------------------------
bool LEDMeterViewCreator::getAttributeNames(StringList& attributeNames) const
{
    attributeNames.emplace_back(kAttrNumSegments);
    attributeNames.emplace_back(kAttrSegmentGap);
    attributeNames.emplace_back(kAttrHorizontal);
    return true;
}

//------------------------------------------------------------------------
IViewCreator::AttrType LEDMeterViewCreator::getAttributeType(const std::string& attributeName) const
{
    if (attributeName == kAttrNumSegments)
        return kIntegerType;
    if (attributeName == kAttrSegmentGap)
        return kFloatType;
    if (attributeName == kAttrHorizontal)
        return kBooleanType;
    return kUnknownType;
}

//------------------------------------------------------------------------
bool LEDMeterViewCreator::getAttributeValue(CView* view, const std::string& attributeName, 
                                             std::string& stringValue, 
                                             const IUIDescription* desc) const
{
    auto* meterView = dynamic_cast<LEDMeterView*>(view);
    if (!meterView)
        return false;
    
    // For now, just return default values
    if (attributeName == kAttrNumSegments)
    {
        stringValue = "12";
        return true;
    }
    if (attributeName == kAttrSegmentGap)
    {
        stringValue = "2";
        return true;
    }
    if (attributeName == kAttrHorizontal)
    {
        stringValue = "true";
        return true;
    }
    return false;
}

//------------------------------------------------------------------------
// ButtonLEDIndicatorCreator Implementation
//------------------------------------------------------------------------
ButtonLEDIndicatorCreator::ButtonLEDIndicatorCreator()
{
    UIViewFactory::registerViewCreator(*this);
}

//------------------------------------------------------------------------
CView* ButtonLEDIndicatorCreator::create(const UIAttributes& attributes, 
                                          const IUIDescription* description) const
{
    CRect size(0, 0, 60, 8);
    return new ButtonLEDIndicator(size);
}

//------------------------------------------------------------------------
bool ButtonLEDIndicatorCreator::apply(CView* view, const UIAttributes& attributes, 
                                       const IUIDescription* description) const
{
    auto* indicator = dynamic_cast<ButtonLEDIndicator*>(view);
    if (!indicator)
        return false;
    
    int32_t intValue;
    if (attributes.getIntegerAttribute(kAttrButtonIndex, intValue))
        indicator->setButtonIndex(intValue);
    
    // Handle color attributes if provided
    const std::string* colorValue = attributes.getAttributeValue(kAttrActiveColor);
    if (colorValue)
    {
        CColor color;
        if (description && UIViewCreator::stringToColor(colorValue, color, description))
            indicator->setActiveColor(color);
    }
    
    colorValue = attributes.getAttributeValue(kAttrInactiveColor);
    if (colorValue)
    {
        CColor color;
        if (description && UIViewCreator::stringToColor(colorValue, color, description))
            indicator->setInactiveColor(color);
    }
    
    return true;
}

//------------------------------------------------------------------------
bool ButtonLEDIndicatorCreator::getAttributeNames(StringList& attributeNames) const
{
    attributeNames.emplace_back(kAttrButtonIndex);
    attributeNames.emplace_back(kAttrActiveColor);
    attributeNames.emplace_back(kAttrInactiveColor);
    return true;
}

//------------------------------------------------------------------------
IViewCreator::AttrType ButtonLEDIndicatorCreator::getAttributeType(const std::string& attributeName) const
{
    if (attributeName == kAttrButtonIndex)
        return kIntegerType;
    if (attributeName == kAttrActiveColor || attributeName == kAttrInactiveColor)
        return kColorType;
    return kUnknownType;
}

//------------------------------------------------------------------------
bool ButtonLEDIndicatorCreator::getAttributeValue(CView* view, const std::string& attributeName, 
                                                   std::string& stringValue, 
                                                   const IUIDescription* desc) const
{
    auto* indicator = dynamic_cast<ButtonLEDIndicator*>(view);
    if (!indicator)
        return false;
    
    if (attributeName == kAttrButtonIndex)
    {
        stringValue = std::to_string(indicator->getButtonIndex());
        return true;
    }
    return false;
}

//------------------------------------------------------------------------
// ButtonSelectionFrameCreator Implementation
//------------------------------------------------------------------------
ButtonSelectionFrameCreator::ButtonSelectionFrameCreator()
{
    UIViewFactory::registerViewCreator(*this);
}

//------------------------------------------------------------------------
CView* ButtonSelectionFrameCreator::create(const UIAttributes& attributes,
                                            const IUIDescription* description) const
{
    CRect size(0, 0, 606, 88);
    return new ButtonSelectionFrame(size);
}

//------------------------------------------------------------------------
bool ButtonSelectionFrameCreator::apply(CView* view, const UIAttributes& attributes,
                                         const IUIDescription* description) const
{
    auto* frame = dynamic_cast<ButtonSelectionFrame*>(view);
    if (!frame)
        return false;
    
    double doubleValue;
    if (attributes.getDoubleAttribute(kAttrFrameWidth, doubleValue))
        frame->setFrameWidth(static_cast<CCoord>(doubleValue));
    
    // Handle frame color
    const std::string* colorValue = attributes.getAttributeValue(kAttrFrameColor);
    if (colorValue)
    {
        CColor color;
        if (description && UIViewCreator::stringToColor(colorValue, color, description))
            frame->setFrameColor(color);
    }
    
    return true;
}

//------------------------------------------------------------------------
bool ButtonSelectionFrameCreator::getAttributeNames(StringList& attributeNames) const
{
    attributeNames.emplace_back(kAttrFrameColor);
    attributeNames.emplace_back(kAttrFrameWidth);
    return true;
}

//------------------------------------------------------------------------
IViewCreator::AttrType ButtonSelectionFrameCreator::getAttributeType(const std::string& attributeName) const
{
    if (attributeName == kAttrFrameColor)
        return kColorType;
    if (attributeName == kAttrFrameWidth)
        return kFloatType;
    return kUnknownType;
}

//------------------------------------------------------------------------
bool ButtonSelectionFrameCreator::getAttributeValue(CView* view, const std::string& attributeName,
                                                     std::string& stringValue,
                                                     const IUIDescription* desc) const
{
    auto* frame = dynamic_cast<ButtonSelectionFrame*>(view);
    if (!frame)
        return false;
    
    if (attributeName == kAttrFrameWidth)
    {
        stringValue = "3";
        return true;
    }
    return false;
}

//------------------------------------------------------------------------
// Static instances to auto-register
//------------------------------------------------------------------------
static LEDMeterViewCreator gLEDMeterViewCreator;
static ButtonLEDIndicatorCreator gButtonLEDIndicatorCreator;
static ButtonSelectionFrameCreator gButtonSelectionFrameCreator;

//------------------------------------------------------------------------
void registerCustomViews()
{
    // Views are auto-registered via static initialization
    // This function can be called to ensure the translation unit is linked
}

//------------------------------------------------------------------------
} // namespace Yonie
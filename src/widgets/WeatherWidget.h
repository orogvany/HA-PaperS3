#pragma once

#include "entity_ref.h"
#include "widgets/Widget.h"
#include <FastEPD.h>

struct WeatherWidgetConfig {
    EntityRef entity_ref;
    const char* label;
    uint16_t pos_x;
    uint16_t pos_y;
    uint16_t width;
    uint16_t height;
};

class WeatherWidget : public Widget {
public:
    WeatherWidget(const char* label, Rect rect);

    void fullDraw(FASTEPD* display, BitDepth depth, const EntityValue& value) override;
    Rect partialDraw(FASTEPD* display, BitDepth depth, const EntityValue& from, const EntityValue& to) override;
    bool isTouching(const TouchEvent* touch_event) const override;
    uint8_t getValueFromTouch(const TouchEvent* touch_event, uint8_t original_value) const override;

private:
    const char* label_;
    Rect rect_;
};

#include "widgets/WeatherWidget.h"
#include "assets/Montserrat_Regular_26.h"
#include "assets/icons.h"
#include <FastEPD.h>
#include <cstdio>

static const uint8_t* condition_icon(WeatherCondition c) {
    switch (c) {
    case WeatherCondition::ClearDay: return weather_clear_day;
    case WeatherCondition::ClearNight: return weather_clear_night;
    case WeatherCondition::Cloudy: return weather_cloudy;
    case WeatherCondition::PartlyCloudy: return weather_partly_cloudy;
    case WeatherCondition::Rainy: return weather_rainy;
    case WeatherCondition::Pouring: return weather_pouring;
    case WeatherCondition::Snowy: return weather_snowy;
    case WeatherCondition::SnowyRainy: return weather_snowy_rainy;
    case WeatherCondition::Fog: return weather_fog;
    case WeatherCondition::Hail: return weather_hail;
    case WeatherCondition::Lightning: return weather_lightning;
    case WeatherCondition::LightningRainy: return weather_lightning_rainy;
    case WeatherCondition::Windy: return weather_windy;
    case WeatherCondition::WindyVariant: return weather_windy_variant;
    case WeatherCondition::Exceptional: return weather_exceptional;
    case WeatherCondition::Unknown: return weather_cloudy;
    }
    return weather_cloudy;
}

static const char* condition_label(WeatherCondition c) {
    switch (c) {
    case WeatherCondition::ClearDay: return "Sunny";
    case WeatherCondition::ClearNight: return "Clear";
    case WeatherCondition::Cloudy: return "Cloudy";
    case WeatherCondition::PartlyCloudy: return "Partly Cloudy";
    case WeatherCondition::Rainy: return "Rainy";
    case WeatherCondition::Pouring: return "Pouring";
    case WeatherCondition::Snowy: return "Snowy";
    case WeatherCondition::SnowyRainy: return "Sleet";
    case WeatherCondition::Fog: return "Foggy";
    case WeatherCondition::Hail: return "Hail";
    case WeatherCondition::Lightning: return "Lightning";
    case WeatherCondition::LightningRainy: return "Thunderstorm";
    case WeatherCondition::Windy: return "Windy";
    case WeatherCondition::WindyVariant: return "Windy";
    case WeatherCondition::Exceptional: return "Extreme";
    case WeatherCondition::Unknown: return "Unknown";
    }
    return "Unknown";
}

WeatherWidget::WeatherWidget(const char* label, Rect rect)
    : label_(label)
    , rect_(rect) {
}

void WeatherWidget::fullDraw(FASTEPD* display, BitDepth depth, const EntityValue& value) {
    const WeatherState& w = value.weather;
    if (w.condition == WeatherCondition::Unknown) return;

    constexpr uint16_t ICON_SIZE = 128;
    constexpr uint16_t ICON_GAP = 12;

    display->setFont(Montserrat_Regular_26);
    display->setTextColor(BBEP_BLACK);

    // Condition icon (left side)
    uint16_t icon_x = rect_.x;
    uint16_t icon_y = rect_.y - 22;
    display->loadBMP(condition_icon(w.condition), icon_x, icon_y, 0xf, BBEP_BLACK);

    // Temperature + condition text (right of icon)
    uint16_t text_x = icon_x + ICON_SIZE + ICON_GAP;

    char temp_buf[16];
    snprintf(temp_buf, sizeof(temp_buf), "%d°%c", w.current_temp, w.temp_unit);
    display->setCursor(text_x, rect_.y + 36);
    display->write(temp_buf);

    BB_RECT tr;
    display->getStringBox(temp_buf, &tr);
    display->setCursor(text_x + tr.w + 16, rect_.y + 36);
    display->write(condition_label(w.condition));

    // Second line: humidity + high/low
    char detail_buf[32] = {};
    int pos = 0;
    if (w.humidity > 0) {
        pos += snprintf(detail_buf + pos, sizeof(detail_buf) - pos, "%d%%", w.humidity);
    }
    if (w.high_temp != 0 || w.low_temp != 0) {
        if (pos > 0) pos += snprintf(detail_buf + pos, sizeof(detail_buf) - pos, "  ");
        snprintf(detail_buf + pos, sizeof(detail_buf) - pos, "H:%d° L:%d°", w.high_temp, w.low_temp);
    }
    if (detail_buf[0]) {
        display->setCursor(text_x, rect_.y + 74);
        display->write(detail_buf);
    }
}

Rect WeatherWidget::partialDraw(FASTEPD* display, BitDepth depth, const EntityValue& from, const EntityValue& to) {
    return {};
}

bool WeatherWidget::isTouching(const TouchEvent* touch_event) const {
    return false;
}

uint8_t WeatherWidget::getValueFromTouch(const TouchEvent* touch_event, uint8_t original_value) const {
    return original_value;
}

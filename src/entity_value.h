#pragma once

#include <cstdint>

enum class EntityValueType : uint8_t {
    Toggle,
    Range,
    Weather,
};

enum class WeatherCondition : uint8_t {
    Unknown,
    ClearDay,
    ClearNight,
    Cloudy,
    PartlyCloudy,
    Rainy,
    Pouring,
    Snowy,
    SnowyRainy,
    Fog,
    Hail,
    Lightning,
    LightningRainy,
    Windy,
    WindyVariant,
    Exceptional,
};

struct WeatherState {
    int16_t current_temp = 0;
    int16_t high_temp = 0;
    int16_t low_temp = 0;
    uint8_t humidity = 0;
    WeatherCondition condition = WeatherCondition::Unknown;
    char temp_unit = 'F';
};

struct EntityValue {
    EntityValueType type = EntityValueType::Toggle;
    union {
        uint8_t toggle;
        uint8_t range;
        WeatherState weather;
    };

    EntityValue() : toggle(0) {}
};

WeatherCondition weather_condition_from_string(const char* state);

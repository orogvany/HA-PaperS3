#include "entity_value.h"
#include <cstring>

WeatherCondition weather_condition_from_string(const char* state) {
    if (strcmp(state, "sunny") == 0) return WeatherCondition::ClearDay;
    if (strcmp(state, "clear-night") == 0) return WeatherCondition::ClearNight;
    if (strcmp(state, "cloudy") == 0) return WeatherCondition::Cloudy;
    if (strcmp(state, "partlycloudy") == 0) return WeatherCondition::PartlyCloudy;
    if (strcmp(state, "rainy") == 0) return WeatherCondition::Rainy;
    if (strcmp(state, "pouring") == 0) return WeatherCondition::Pouring;
    if (strcmp(state, "snowy") == 0) return WeatherCondition::Snowy;
    if (strcmp(state, "snowy-rainy") == 0) return WeatherCondition::SnowyRainy;
    if (strcmp(state, "fog") == 0) return WeatherCondition::Fog;
    if (strcmp(state, "hail") == 0) return WeatherCondition::Hail;
    if (strcmp(state, "lightning") == 0) return WeatherCondition::Lightning;
    if (strcmp(state, "lightning-rainy") == 0) return WeatherCondition::LightningRainy;
    if (strcmp(state, "windy") == 0) return WeatherCondition::Windy;
    if (strcmp(state, "windy-variant") == 0) return WeatherCondition::WindyVariant;
    if (strcmp(state, "exceptional") == 0) return WeatherCondition::Exceptional;
    return WeatherCondition::Unknown;
}

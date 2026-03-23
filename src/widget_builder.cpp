#include "widget_builder.h"
#include "assets/icons.h"
#include "boards.h"
#include "entity_value.h"
#include "esp_log.h"
#include "widgets/WeatherWidget.h"
#include <cstring>

static const char* TAG = "widget_builder";

// Map icon name string to compiled icon array
struct IconEntry {
    const char* name;
    const uint8_t* data;
};

static const IconEntry icon_table[] = {
    {"lightbulb", btn_lightbulb}, {"lightbulb_off", btn_lightbulb_off},
    {"ceiling_light", btn_ceiling_light}, {"ceiling_light_off", btn_ceiling_light_off},
    {"floor_lamp", btn_floor_lamp}, {"floor_lamp_off", btn_floor_lamp_off},
    {"desk_lamp", btn_desk_lamp}, {"desk_lamp_off", btn_desk_lamp_off},
    {"table_lamp", btn_table_lamp}, {"table_lamp_off", btn_table_lamp_off},
    {"chandelier", btn_chandelier}, {"chandelier_off", btn_chandelier_off},
    {"fan", btn_fan}, {"fan_off", btn_fan_off},
    {"ceiling_fan", btn_ceiling_fan}, {"ceiling_fan_off", btn_ceiling_fan_off},
    {"fan_and_light", btn_fan_and_light}, {"fan_and_light_off", btn_fan_and_light_off},
    {"air_conditioner", btn_air_conditioner}, {"air_conditioner_off", btn_air_conditioner_off},
    {"heater", btn_heater}, {"heater_off", btn_heater_off},
    {"air_purifier", btn_air_purifier}, {"air_purifier_off", btn_air_purifier_off},
    {"outlet", btn_outlet}, {"outlet_off", btn_outlet_off},
    {"light_switch", btn_light_switch}, {"light_switch_off", btn_light_switch_off},
    {"power", btn_power}, {"power_off", btn_power_off},
    {"blinds", btn_blinds}, {"blinds_off", btn_blinds_off},
    {"curtains", btn_curtains}, {"curtains_off", btn_curtains_off},
    {"garage", btn_garage}, {"garage_off", btn_garage_off},
    {"door", btn_door}, {"door_off", btn_door_off},
    {"lock", btn_lock}, {"lock_off", btn_lock_off},
    {"smart_lock", btn_smart_lock}, {"smart_lock_off", btn_smart_lock_off},
    {"speaker", btn_speaker}, {"speaker_off", btn_speaker_off},
    {"display", btn_display}, {"display_off", btn_display_off},
    {"thermometer", btn_thermometer}, {"thermometer_off", btn_thermometer_off},
    {"humidity_icon", btn_humidity_icon}, {"humidity_icon_off", btn_humidity_icon_off},
    {"sensor", btn_sensor}, {"sensor_off", btn_sensor_off},
    {"contact_sensor", btn_contact_sensor}, {"contact_sensor_off", btn_contact_sensor_off},
    {"bolt", btn_bolt}, {"bolt_off", btn_bolt_off},
    {"house", btn_house}, {"house_off", btn_house_off},
    {"fireplace", btn_fireplace}, {"fireplace_off", btn_fireplace_off},
    {"flame", btn_flame}, {"flame_off", btn_flame_off},
    {"snowflake", btn_snowflake}, {"snowflake_off", btn_snowflake_off},
    {"wifi_router", btn_wifi_router}, {"wifi_router_off", btn_wifi_router_off},
    {"camera", btn_camera}, {"camera_off", btn_camera_off},
    {"printer", btn_printer}, {"printer_off", btn_printer_off},
    {"laptop", btn_laptop}, {"laptop_off", btn_laptop_off},
    {"car", btn_car}, {"car_off", btn_car_off},
    {"default", btn_default}, {"default_off", btn_default_off},
};

static const uint8_t* icon_by_name(const char* name) {
    for (const auto& entry : icon_table) {
        if (strcmp(name, entry.name) == 0) return entry.data;
    }
    return btn_lightbulb;
}

static const char* default_icon_on(const char* domain) {
    if (strcmp(domain, "light") == 0) return "lightbulb";
    if (strcmp(domain, "fan") == 0) return "fan";
    if (strcmp(domain, "switch") == 0) return "outlet";
    if (strcmp(domain, "cover") == 0) return "blinds";
    if (strcmp(domain, "lock") == 0) return "lock";
    if (strcmp(domain, "media_player") == 0) return "speaker";
    if (strcmp(domain, "climate") == 0) return "air_conditioner";
    if (strcmp(domain, "sensor") == 0) return "thermometer";
    if (strcmp(domain, "binary_sensor") == 0) return "sensor";
    if (strcmp(domain, "scene") == 0) return "house";
    if (strcmp(domain, "script") == 0) return "bolt";
    if (strcmp(domain, "automation") == 0) return "bolt";
    if (strcmp(domain, "input_boolean") == 0) return "light_switch";
    if (strcmp(domain, "input_number") == 0) return "power";
    if (strcmp(domain, "vacuum") == 0) return "fan";
    return "lightbulb";
}

static const char* default_icon_off(const char* domain) {
    if (strcmp(domain, "light") == 0) return "lightbulb_off";
    if (strcmp(domain, "fan") == 0) return "fan_off";
    if (strcmp(domain, "switch") == 0) return "outlet_off";
    if (strcmp(domain, "cover") == 0) return "blinds_off";
    if (strcmp(domain, "lock") == 0) return "lock_off";
    if (strcmp(domain, "media_player") == 0) return "speaker_off";
    if (strcmp(domain, "climate") == 0) return "air_conditioner_off";
    if (strcmp(domain, "sensor") == 0) return "thermometer_off";
    if (strcmp(domain, "binary_sensor") == 0) return "sensor_off";
    if (strcmp(domain, "scene") == 0) return "house_off";
    if (strcmp(domain, "script") == 0) return "bolt_off";
    if (strcmp(domain, "automation") == 0) return "bolt_off";
    if (strcmp(domain, "input_boolean") == 0) return "light_switch_off";
    if (strcmp(domain, "input_number") == 0) return "power_off";
    if (strcmp(domain, "vacuum") == 0) return "fan_off";
    return "lightbulb_off";
}

// Map entity_id domain to CommandType
static CommandType domain_to_command_type(const char* entity_id, const char* widget_type) {
    // Extract domain from "domain.name"
    char domain[16] = {};
    const char* dot = strchr(entity_id, '.');
    if (dot) {
        size_t len = dot - entity_id;
        if (len >= sizeof(domain)) len = sizeof(domain) - 1;
        memcpy(domain, entity_id, len);
    }

    if (strcmp(domain, "light") == 0) {
        return strcmp(widget_type, "slider") == 0
            ? CommandType::SetLightBrightnessPercentage
            : CommandType::SwitchOnOff;
    }
    if (strcmp(domain, "switch") == 0) return CommandType::SwitchOnOff;
    if (strcmp(domain, "fan") == 0) return CommandType::SetFanSpeedPercentage;
    if (strcmp(domain, "cover") == 0) return CommandType::SetCoverPosition;
    if (strcmp(domain, "scene") == 0) return CommandType::ActivateScene;
    if (strcmp(domain, "script") == 0) return CommandType::RunScript;
    if (strcmp(domain, "lock") == 0) return CommandType::LockUnlock;
    if (strcmp(domain, "media_player") == 0) {
        return strcmp(widget_type, "slider") == 0
            ? CommandType::SetMediaPlayerVolume
            : CommandType::MediaPlayerPlayPause;
    }
    if (strcmp(domain, "input_number") == 0) return CommandType::SetInputNumber;
    if (strcmp(domain, "input_boolean") == 0) return CommandType::InputBooleanToggle;
    if (strcmp(domain, "automation") == 0) return CommandType::AutomationOnOff;
    if (strcmp(domain, "vacuum") == 0) return CommandType::VacuumCommand;

    return CommandType::SwitchOnOff; // Fallback
}

int build_widgets_from_config(const AppConfig& app, EntityStore* store, Screen* screen) {
    if (app.ui_device_count == 0) return 0;

    // Auto-layout: stack widgets vertically
    constexpr uint16_t margin = 30;
    constexpr uint16_t slider_height = 170;
    constexpr uint16_t button_spacing = 140; // Button height + gap
    constexpr uint16_t slider_spacing = slider_height + 40;
    uint16_t y = margin;

    constexpr uint16_t weather_height = 160;
    constexpr uint16_t weather_spacing = weather_height + 20;

    int count = 0;
    for (int i = 0; i < app.ui_device_count && i < MAX_WIDGETS_PER_SCREEN; i++) {
        const UIDevice& dev = app.ui_devices[i];
        bool is_slider = (strcmp(dev.widget_type, "slider") == 0);
        bool is_weather = (strcmp(dev.widget_type, "weather") == 0);
        bool is_alexa = (strcmp(dev.source, "alexa") == 0);

        if (is_weather) {
            EntityConfig entity = {
                .entity_id = dev.entity_id,
                .command_type = CommandType::SwitchOnOff,
                .value_type = EntityValueType::Weather,
                .source = is_alexa ? EntitySource::Alexa : EntitySource::HomeAssistant,
                .read_only = true,
            };

            screen_add_weather(
                WeatherWidgetConfig{
                    .entity_ref = store_add_entity(store, entity),
                    .label = dev.label,
                    .pos_x = margin,
                    .pos_y = y,
                    .width = (uint16_t)(DISPLAY_WIDTH - 2 * margin),
                    .height = weather_height,
                },
                screen);
            y += weather_spacing;
        } else {
            CommandType cmd_type;
            if (is_alexa) {
                cmd_type = is_slider ? CommandType::AlexaSetBrightness : CommandType::AlexaSetPower;
            } else {
                cmd_type = domain_to_command_type(dev.entity_id, dev.widget_type);
            }

            EntityConfig entity = {
                .entity_id = dev.entity_id,
                .command_type = cmd_type,
                .source = is_alexa ? EntitySource::Alexa : EntitySource::HomeAssistant,
            };

            // Extract domain for default icon lookup
            char domain[16] = {};
            const char* dot = strchr(dev.entity_id, '.');
            if (dot) { size_t len = dot - dev.entity_id; if (len >= sizeof(domain)) len = sizeof(domain)-1; memcpy(domain, dev.entity_id, len); }

            const uint8_t* ico_on = icon_by_name(dev.icon_on[0] ? dev.icon_on : default_icon_on(domain));
            const uint8_t* ico_off = icon_by_name(dev.icon_off[0] ? dev.icon_off : default_icon_off(domain));

            if (is_slider) {
                screen_add_slider(
                    SliderConfig{
                        .entity_ref = store_add_entity(store, entity),
                        .label = dev.label,
                        .icon_on = ico_on,
                        .icon_off = ico_off,
                        .pos_x = margin,
                        .pos_y = y,
                        .width = (uint16_t)(DISPLAY_WIDTH - 2 * margin),
                        .height = slider_height,
                    },
                    screen);
                y += slider_spacing;
            } else {
                screen_add_button(
                    ButtonConfig{
                        .entity_ref = store_add_entity(store, entity),
                        .label = dev.label,
                        .icon_on = ico_on,
                        .icon_off = ico_off,
                        .pos_x = margin,
                        .pos_y = y,
                    },
                    screen);
                y += button_spacing;
            }
        }

        ESP_LOGI(TAG, "Widget %d: %s (%s) at y=%d", count, dev.entity_id, dev.widget_type, y);
        count++;
    }

    ESP_LOGI(TAG, "Built %d widgets from config", count);
    return count;
}

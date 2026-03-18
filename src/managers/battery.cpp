#include "battery.h"
#include "boards.h"
#include "constants.h"
#include "esp_log.h"
#include <Arduino.h>

static const char* TAG = "battery";

struct BatteryLookupEntry {
    uint16_t voltage_mv;
    uint8_t percentage;
};

static const BatteryLookupEntry battery_curve[] = {
    {4200, 100},
    {4150, 95},
    {4110, 90},
    {4080, 85},
    {4020, 80},
    {3980, 70},
    {3950, 60},
    {3910, 50},
    {3870, 40},
    {3830, 30},
    {3790, 20},
    {3750, 15},
    {3700, 10},
    {3600, 5},
    {3300, 0},
};

static const size_t CURVE_SIZE = sizeof(battery_curve) / sizeof(battery_curve[0]);

static uint8_t voltage_to_percentage(uint16_t voltage_mv) {
    if (voltage_mv >= battery_curve[0].voltage_mv) return 100;
    if (voltage_mv <= battery_curve[CURVE_SIZE - 1].voltage_mv) return 0;

    for (size_t i = 0; i < CURVE_SIZE - 1; i++) {
        if (voltage_mv >= battery_curve[i + 1].voltage_mv) {
            uint16_t v_high = battery_curve[i].voltage_mv;
            uint16_t v_low = battery_curve[i + 1].voltage_mv;
            uint8_t p_high = battery_curve[i].percentage;
            uint8_t p_low = battery_curve[i + 1].percentage;
            return p_low + (uint8_t)((uint32_t)(voltage_mv - v_low) * (p_high - p_low) / (v_high - v_low));
        }
    }

    return 0;
}

void battery_task(void* arg) {
    BatteryTaskArgs* ctx = static_cast<BatteryTaskArgs*>(arg);

    if (!HAS_BATTERY_ADC) {
        ESP_LOGI(TAG, "No battery ADC on this board, suspending task");
        vTaskSuspend(nullptr);
    }

    pinMode(BATTERY_CHARGE_PIN, INPUT);

    while (true) {
        uint16_t raw_mv = analogReadMilliVolts(BATTERY_ADC_PIN);
        uint16_t voltage_mv = (uint16_t)(raw_mv * BATTERY_ADC_DIVIDER_RATIO);
        uint8_t percentage = voltage_to_percentage(voltage_mv);
        bool charging = (digitalRead(BATTERY_CHARGE_PIN) == LOW);

        ESP_LOGI(TAG, "Battery: %d mV (%d%%)%s", voltage_mv, percentage, charging ? " [charging]" : "");

        store_set_battery(ctx->store, voltage_mv, percentage, charging);

        vTaskDelay(pdMS_TO_TICKS(BATTERY_READ_INTERVAL_MS));
    }
}

#pragma once

#include <cstddef>
#include <cstdint>

// ============================================================================
// PHASE FEATURE FLAGS — set to false to disable individual power saving phases
// Start with all OFF for baseline testing, enable one at a time
// ============================================================================
constexpr bool PHASE1_WIFI_MODEM_SLEEP = false;     // WiFi modem power save between DTIM beacons
constexpr bool PHASE2_LIGHT_SLEEP = false;           // CPU light sleep via idle hook + interrupt-driven touch
constexpr bool PHASE3_IDLE_WIFI_DISCONNECT = false;  // Disconnect WiFi after idle timeout
constexpr bool PHASE4_PMS150G_SHUTDOWN = false;      // Deep power-off via PMS150G after extended idle
constexpr bool FEATURE_BATTERY_INDICATOR = false;    // On-screen battery percentage
constexpr bool FEATURE_BMI270_SUSPEND = false;       // Put gyroscope in suspend mode on boot

// Buttons configuration
constexpr uint8_t BUTTON_BORDER_SIZE = 4;
constexpr uint8_t BUTTON_SIZE = 100;
constexpr uint8_t BUTTON_ICON_SIZE = 64;
constexpr uint8_t SLIDER_OFFSET = 100;    // The zero is a bit on the right
constexpr uint8_t TOUCH_AREA_MARGIN = 15; // A touch within 15px of the target is OK

// Home assistant configuration
constexpr uint16_t HASS_MAX_JSON_BUFFER = 1024 * 20; // 20k, home assistant talks a lot
constexpr uint32_t HASS_RECONNECT_DELAY_MS = 10000;

// When sending commands too fast (on a slider), this can flood
// the zigbee network and make the commands fail. Increase this delay
// if you see errors when using sliders.
constexpr uint32_t HASS_TASK_SEND_DELAY_MS = 500;

// When sending commands, we'll receive the updates from the server
// with a delay. This causes jittering in the slider and unnecessary
// commands sent to the server. We ignore updates from the server
// during this delay after a command was sent on an entity.
// FIXME: We can lose updates, we should have an authoritative value
// and a target value in the store at some point.
constexpr uint32_t HASS_IGNORE_UPDATE_DELAY_MS = 1000;

// Battery monitoring
constexpr uint32_t BATTERY_READ_INTERVAL_MS = 5 * 60 * 1000; // 5 minutes
constexpr uint8_t BATTERY_ADC_PIN = 3;
constexpr uint8_t BATTERY_CHARGE_PIN = 4;
constexpr float BATTERY_ADC_DIVIDER_RATIO = 2.0f;

// Touch polling intervals
constexpr uint32_t TOUCH_POLL_ACTIVE_MS = 25;
constexpr uint32_t TOUCH_POLL_IDLE_MS = 500;

// Light sleep wake interval for servicing WebSocket when idle
constexpr uint32_t SLEEP_WAKE_INTERVAL_MS = 5000;

// Idle WiFi disconnect timeout (disconnect WiFi after no touch for this long)
constexpr uint32_t IDLE_WIFI_DISCONNECT_MS = 5 * 60 * 1000; // 5 minutes

// PMS150G auto-shutdown (Phase 4 deep power-off)
constexpr bool PMS150G_AUTO_SHUTDOWN_ENABLED = true;
constexpr uint32_t PMS150G_SHUTDOWN_IDLE_MS = 6UL * 60 * 60 * 1000;  // 6 hours
constexpr uint8_t PMS150G_RTC_WAKE_INTERVAL_MIN = 240;               // 4 hours (max 255)

// Touch feedback
constexpr bool BUZZER_FEEDBACK_ENABLED = true;
constexpr uint16_t BUZZER_FREQ_HZ = 3000;
constexpr uint8_t BUZZER_DURATION_MS = 15;

// Other constants
constexpr size_t MAX_ENTITIES = 16;
constexpr size_t MAX_WIDGETS_PER_SCREEN = 8;
constexpr uint32_t TOUCH_RELEASE_TIMEOUT_MS = 50;
constexpr uint32_t DISPLAY_FULL_REDRAW_TIMEOUT_MS = 30000;

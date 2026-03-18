#include "boards.h"
#include "config_remote.h"
#include "constants.h"
#include "draw.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "esp_freertos_hooks.h"
#include <Wire.h>
#include "managers/battery.h"
#include "managers/power.h"
#include "managers/home_assistant.h"
#include "managers/touch.h"
#include "managers/ui.h"
#include "managers/wifi.h"
#include "screen.h"
#include "store.h"
#include "ui_state.h"
#include "widgets/Slider.h"
#include <FastEPD.h>

static Configuration config;

static FASTEPD epaper;
static Screen screen;
static BBCapTouch bbct;
static EntityStore store;
static SharedUIState shared_ui_state;

static UITaskArgs ui_task_args;
static TouchTaskArgs touch_task_args;
static HomeAssistantTaskArgs hass_task_args;
static BatteryTaskArgs battery_task_args;

static bool idle_hook() {
    esp_light_sleep_start();
    return true;
}

void setup() {
    Wire.begin(TOUCH_SDA, TOUCH_SCL);

    // Put BMI270 gyroscope into suspend mode (~3.5µA vs ~950µA)
    Wire.beginTransmission(0x68);
    Wire.write(0x7D); // PWR_CTRL register
    Wire.write(0x00); // Disable accelerometer and gyroscope
    Wire.endTransmission();

    // Check if this is an RTC wake from PMS150G power-off
    if (HAS_PMS150G && PMS150G_AUTO_SHUTDOWN_ENABLED && power_was_rtc_wake()) {
        power_clear_rtc_flag();

        epaper.initPanel(DISPLAY_PANEL);
        epaper.setPanelSize(DISPLAY_HEIGHT, DISPLAY_WIDTH);
        epaper.setRotation(90);
        epaper.einkPower(true);

        // Read RTC seconds (BCD-encoded, bit 7 = VL flag) for position offset
        Wire.beginTransmission(BM8563_ADDR);
        Wire.write(0x02); // Seconds register
        Wire.endTransmission(false);
        Wire.requestFrom((uint8_t)BM8563_ADDR, (uint8_t)1);
        uint8_t raw = Wire.available() ? Wire.read() : 0;
        uint8_t rtc_sec = ((raw & 0x70) >> 4) * 10 + (raw & 0x0F);
        int16_t offset_x = (int16_t)((rtc_sec * 7) % 100) - 50;
        int16_t offset_y = (int16_t)((rtc_sec * 13) % 80) - 40;

        drawIdleScreen(&epaper, offset_x, offset_y);

        power_setup_rtc_timer(PMS150G_RTC_WAKE_INTERVAL_MIN);
        power_off_pms150g();
        // If power-off failed (e.g., USB connected), fall through to normal boot
    }

    // Configure light sleep wake sources
    esp_sleep_enable_timer_wakeup(SLEEP_WAKE_INTERVAL_MS * 1000);
    gpio_wakeup_enable((gpio_num_t)TOUCH_INT, GPIO_INTR_LOW_LEVEL);
    esp_sleep_enable_gpio_wakeup();

    // Register idle hook to enter light sleep when all tasks are blocked
    esp_register_freertos_idle_hook_for_cpu(idle_hook, 0);
    esp_register_freertos_idle_hook_for_cpu(idle_hook, 1);

    // Initialize objects
    store_init(&store);
    ui_state_init(&shared_ui_state);
    configure_remote(&config, &store, &screen);
    initialize_slider_sprites();

    // Initialize display
    epaper.initPanel(DISPLAY_PANEL);
    epaper.setPanelSize(DISPLAY_HEIGHT, DISPLAY_WIDTH);
    epaper.setRotation(90);
    epaper.einkPower(true); // FIXME: Disabling power makes the GT911 unavailable

    // Launch UI task
    ui_task_args.epaper = &epaper;
    ui_task_args.screen = &screen;
    ui_task_args.store = &store;
    ui_task_args.shared_state = &shared_ui_state;
    xTaskCreate(ui_task, "ui", 4096, &ui_task_args, 1, &store.ui_task);

    // Connect to wifi and launch watcher
    launch_wifi(&config, &store);

    // Connect to home assistant
    hass_task_args.config = &config;
    hass_task_args.store = &store;
    hass_task_args.epaper = &epaper;
    xTaskCreate(home_assistant_task, "home_assistant", 8192, &hass_task_args, 1, &store.home_assistant_task);

    // Launch touch task
    touch_task_args.bbct = &bbct;
    touch_task_args.screen = &screen;
    touch_task_args.state = &shared_ui_state;
    touch_task_args.store = &store;
    xTaskCreate(touch_task, "touch", 4096, &touch_task_args, 1, nullptr);

    // Launch battery monitoring task
    battery_task_args.store = &store;
    xTaskCreate(battery_task, "battery", 2048, &battery_task_args, 1, nullptr);
}

void loop() {
    vTaskDelay(portMAX_DELAY); // Nothing to do here
}

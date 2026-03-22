#include "wifi_portal.h"
#include "esp_log.h"
#include <WiFi.h>
#include <WiFiManager.h>

static const char* TAG = "wifi_portal";
static WiFiManager wm;
static bool portal_active = false;

void wifi_portal_start(const char* ap_name) {
    if (portal_active) return;

    ESP_LOGI(TAG, "Starting WiFi captive portal AP: %s", ap_name);

    // Disconnect existing WiFi so the portal doesn't instantly detect "connected"
    WiFi.disconnect(true);
    delay(100);

    wm.setConfigPortalBlocking(false);
    wm.startConfigPortal(ap_name);
    portal_active = true;
}

void wifi_portal_stop() {
    if (!portal_active) return;

    ESP_LOGI(TAG, "Stopping WiFi captive portal");
    wm.stopConfigPortal();
    portal_active = false;
}

bool wifi_portal_poll() {
    if (!portal_active) return false;

    wm.process();

    if (WiFi.status() == WL_CONNECTED) {
        ESP_LOGI(TAG, "WiFi connected via portal! SSID: %s, IP: %s",
                 WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
        portal_active = false;
        return true;
    }

    return false;
}

bool wifi_portal_is_active() {
    return portal_active;
}

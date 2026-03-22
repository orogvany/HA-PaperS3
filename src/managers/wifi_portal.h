#pragma once

// Start the WiFiManager captive portal AP.
// Call wifi_portal_poll() periodically while active.
// Returns true if WiFi was successfully configured.
void wifi_portal_start(const char* ap_name);
void wifi_portal_stop();
bool wifi_portal_poll(); // Returns true when WiFi connected
bool wifi_portal_is_active();

#include "ha_rest_client.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

static const char* TAG = "ha_rest";

void HARestClient::begin(const HAConfig& config) {
    _config = config;
}

String HARestClient::_buildUrl(const char* path) {
    return String(_config.base_url) + path;
}

bool HARestClient::_doGet(const char* url, String& response) {
    HTTPClient http;
    http.begin(url);
    http.addHeader("Authorization", String("Bearer ") + _config.token);
    http.addHeader("Content-Type", "application/json");

    int code = http.GET();
    if (code == 200) {
        response = http.getString();
        http.end();
        return true;
    }

    ESP_LOGE(TAG, "GET %s failed: %d", url, code);
    http.end();
    return false;
}

bool HARestClient::_doPost(const char* url, const String& body, String& response) {
    HTTPClient http;
    http.begin(url);
    http.addHeader("Authorization", String("Bearer ") + _config.token);
    http.addHeader("Content-Type", "application/json");

    int code = http.POST(body);
    if (code >= 200 && code < 300) {
        response = http.getString();
        http.end();
        return true;
    }

    ESP_LOGE(TAG, "POST %s failed: %d", url, code);
    http.end();
    return false;
}

int HARestClient::testConnection() {
    HTTPClient http;
    String url = _buildUrl("/api/");
    http.begin(url);
    http.addHeader("Authorization", String("Bearer ") + _config.token);
    int code = http.GET();
    http.end();
    return code;
}

bool HARestClient::getEntityState(const char* entity_id, HAEntityState* out) {
    String url = _buildUrl("/api/states/") + entity_id;
    String response;
    if (!_doGet(url.c_str(), response)) return false;

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, response);
    if (err) {
        ESP_LOGE(TAG, "JSON parse error: %s", err.c_str());
        return false;
    }

    strlcpy(out->entity_id, doc["entity_id"] | "", sizeof(out->entity_id));
    strlcpy(out->state, doc["state"] | "", sizeof(out->state));

    JsonObject attrs = doc["attributes"];
    strlcpy(out->friendly_name, attrs["friendly_name"] | "", sizeof(out->friendly_name));
    out->brightness = attrs["brightness"] | -1;
    out->percentage = attrs["percentage"] | -1;
    out->current_position = attrs["current_position"] | -1;
    out->volume_level = attrs["volume_level"] | -1.0;

    return true;
}

int HARestClient::getAllStates(void (*callback)(const HAEntityState& state, void* ctx), void* ctx) {
    String url = _buildUrl("/api/states");
    String response;
    if (!_doGet(url.c_str(), response)) return -1;

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, response);
    if (err) {
        ESP_LOGE(TAG, "JSON parse error for /api/states: %s", err.c_str());
        return -1;
    }

    JsonArray arr = doc.as<JsonArray>();
    int count = 0;
    for (JsonObject obj : arr) {
        HAEntityState state = {};
        strlcpy(state.entity_id, obj["entity_id"] | "", sizeof(state.entity_id));
        strlcpy(state.state, obj["state"] | "", sizeof(state.state));

        JsonObject attrs = obj["attributes"];
        strlcpy(state.friendly_name, attrs["friendly_name"] | "", sizeof(state.friendly_name));
        state.brightness = attrs["brightness"] | -1;
        state.percentage = attrs["percentage"] | -1;
        state.current_position = attrs["current_position"] | -1;
        state.volume_level = attrs["volume_level"] | -1.0;

        callback(state, ctx);
        count++;
    }

    return count;
}

bool HARestClient::callService(const char* domain, const char* service,
                                const char* entity_id, const char* extra_json) {
    String url = _buildUrl("/api/services/");
    url += domain;
    url += "/";
    url += service;

    String body = "{\"entity_id\":\"";
    body += entity_id;
    body += "\"";
    if (extra_json && extra_json[0]) {
        body += ",";
        body += extra_json;
    }
    body += "}";

    ESP_LOGI(TAG, "POST %s body: %s", url.c_str(), body.c_str());

    String response;
    return _doPost(url.c_str(), body, response);
}

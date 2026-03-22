#pragma once
#include "device_interface.h"
#include "ha_rest_client.h"

// HA-specific implementation of DeviceAction.
// Translates generic commands (sendCommand with value 0-100) into
// the correct HA REST API service calls for each device type.

class HADeviceAction : public DeviceAction {
public:
    HADeviceAction(HARestClient* client, const char* entity_id,
                   DeviceType type, const char* friendly_name);

    bool sendCommand(uint8_t value) override;
    DeviceState pollState() override;
    const char* getEntityId() override { return _entity_id; }
    DeviceType getDeviceType() override { return _type; }
    const char* getFriendlyName() override { return _friendly_name; }

private:
    HARestClient* _client;
    char _entity_id[64];
    char _friendly_name[48];
    DeviceType _type;

    // Extract domain from entity_id (e.g., "light" from "light.living_room")
    const char* _getDomain();

    // Parse HA state string into our DeviceState
    DeviceState _parseState(const HAEntityState& ha_state);
};

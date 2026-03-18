#pragma once
#include "store.h"

struct BatteryTaskArgs {
    EntityStore* store;
};

void battery_task(void* arg);

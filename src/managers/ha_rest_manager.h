#pragma once
#include "config.h"
#include "config_store.h"
#include "store.h"
#include <FastEPD.h>

struct HARestManagerArgs {
    EntityStore* store;
    Configuration* config;
    ConfigStore* config_store;
    FASTEPD* epaper;
};

void ha_rest_manager_task(void* arg);

#pragma once
#include "config.h"
#include "store.h"
#include <FastEPD.h>

struct HARestManagerArgs {
    EntityStore* store;
    Configuration* config;
    FASTEPD* epaper;
};

void ha_rest_manager_task(void* arg);

#pragma once

#include "config.h"
#include "store.h"
#include <FastEPD.h>

struct HomeAssistantTaskArgs {
    EntityStore* store;
    Configuration* config;
    FASTEPD* epaper;
};

void home_assistant_task(void* arg);
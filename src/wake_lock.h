#pragma once
#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Simple wake lock mechanism for light sleep.
// Any task doing hardware I/O (SPI, I2C, WiFi) should hold a wake lock
// to prevent the idle hook from entering light sleep mid-operation.
//
// Usage:
//   wake_lock_acquire();
//   // ... do SPI/I2C/WiFi operation ...
//   wake_lock_release();

void wake_lock_init();
void wake_lock_acquire();
void wake_lock_release();
bool wake_lock_is_held();

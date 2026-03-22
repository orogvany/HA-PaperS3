#include "wake_lock.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static volatile int32_t lock_count = 0;
static portMUX_TYPE lock_spinlock = portMUX_INITIALIZER_UNLOCKED;

void wake_lock_init() {
    lock_count = 0;
}

void wake_lock_acquire() {
    portENTER_CRITICAL(&lock_spinlock);
    lock_count++;
    portEXIT_CRITICAL(&lock_spinlock);
}

void wake_lock_release() {
    portENTER_CRITICAL(&lock_spinlock);
    if (lock_count > 0) lock_count--;
    portEXIT_CRITICAL(&lock_spinlock);
}

bool wake_lock_is_held() {
    return lock_count > 0;
}

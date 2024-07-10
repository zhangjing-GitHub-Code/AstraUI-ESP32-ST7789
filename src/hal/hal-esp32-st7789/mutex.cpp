#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "mutex.h"


#ifndef MUTEX_C
#define MUTEX_C

namespace mutex{
SemaphoreHandle_t Fmutex;
lockStat ulock;
async_memcpy_t AMdri;
};
SPR_TYPE *glgfx;
// void WAIT_LOCK(mutex::lockStat x){while(mutex::ulock!=x){vTaskDelay(1);}}
#endif
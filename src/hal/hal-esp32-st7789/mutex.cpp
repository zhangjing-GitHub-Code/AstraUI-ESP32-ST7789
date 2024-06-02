#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "mutex.h"
#include "AGFX-CM-Enhance.h"

#ifndef MUTEX_C
#define MUTEX_C

namespace mutex{
SemaphoreHandle_t Fmutex;
bool isLock=0;
async_memcpy_t AMdri;
};
ACME *gtgfx;

#endif
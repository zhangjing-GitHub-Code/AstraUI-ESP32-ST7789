#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "mutex.h"
#include "AGFX-CM-Enhance.h"

#ifndef MUTEX_C
#define MUTEX_C

namespace mutex{
SemaphoreHandle_t Fmutex;
bool isLock=0;
};
Arduino_Canvas_Idx_Enhanced *gtgfx;

#endif
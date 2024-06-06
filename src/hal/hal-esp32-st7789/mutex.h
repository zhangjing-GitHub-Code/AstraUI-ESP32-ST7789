#pragma once
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "esp_async_memcpy.h"

#ifndef MUTEX_H
#define MUTEX_H


#include "AGFX-CM-Enhance.h"

namespace mutex{
// WAIT until x is true
#define WAIT_LOCK(x){while(mutex::ulock!=x){vTaskDelay(1);}}
// extern enum lockStat;
enum lockStat{
		FREE,
		FLUSHING,
		CLEARING,
		HELD
	};
extern SemaphoreHandle_t Fmutex;
extern lockStat ulock;
extern async_memcpy_t AMdri;
};
extern ACME *gtgfx;

// void WAIT_LOCK(mutex::lockStat x);
#endif
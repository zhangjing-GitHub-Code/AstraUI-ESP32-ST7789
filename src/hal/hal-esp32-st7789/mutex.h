#pragma once
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include "esp_async_memcpy.h"

#ifndef MUTEX_H
#define MUTEX_H

#include "AGFX-CM-Enhance.h"

namespace mutex{
extern SemaphoreHandle_t Fmutex;
extern bool isLock;
extern async_memcpy_t AMdri;
};
extern ACME *gtgfx;

#endif
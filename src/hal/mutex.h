#pragma once
#include <Arduino.h>
#include <Arduino_GFX_Library.h>

#ifndef MUTEX_H
#define MUTEX_H

#include "AGFX-CM-Enhance.h"

namespace mutex{
extern SemaphoreHandle_t Fmutex;
extern bool isLock;
};
extern Arduino_Canvas_Idx_Enhanced *gtgfx;

#endif
#pragma once

#include <SDL3/SDL.h>
#include <sysinfoapi.h>
#include <windows.h>
#include <stdio.h>
#include "window.h"

int DR_Start();
int DR_Run();
int DR_GameLoop();
void DR_LoopThrottle(ULONGLONG* prev_time, float interval);
void DR_GameLogic();
void DR_WindowEvents();
void DR_FPSCounter(int* fps_counter);
ULONGLONG millis();

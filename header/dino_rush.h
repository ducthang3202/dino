#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#ifdef _WIN32
  #include <sysinfoapi.h>
  #include <windows.h>
  #include <malloc.h>
#else
  #include <unistd.h>
#endif

#include <time.h>
#include <stdio.h>
#include <math.h>

#include "game_object.h"
#include "window.h"
#include "ll.h"
#include "level.h"

int DR_Start();
uint64_t millis();
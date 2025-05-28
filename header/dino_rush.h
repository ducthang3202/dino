#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <sysinfoapi.h>
#include <windows.h>
#include <malloc.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "game_object.h"
#include "window.h"
#include "ll.h"
#include "level.h"

extern Dino* dino;
extern int high_score;
extern float angle;
extern bool show_hitbox;

int DR_Start();
ULONGLONG millis();
int DR_TitleScreen();


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
int DR_Run();
int DR_GameLoop();
int DR_Init();
ULONGLONG millis();

void DR_LoopThrottle(ULONGLONG* prev_time, float interval);
void DR_GameLogic();
void DR_Draw();
void DR_DrawText(int x, int y, bool left_centered, char* text_buff,SDL_Color col, TTF_Font* font, SDL_Surface** text_sur);



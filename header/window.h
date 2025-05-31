#pragma once
#include <SDL3/SDL.h>
#include <stdio.h>
#include "dino_rush.h"

#ifndef DESIRED_FPS
#define DESIRED_FPS 120
#endif

#define PI M_PI
#define GAME_WINDOW_EVENT_NOERR 0
#define GAME_WINDOW_EVENT_EXIT_SAFE 1
#define GAME_WINDOW_EVENT_EXIT_CRASH -1
#define GAME_WINDOW_WIDTH 1000
#define GAME_WINDOW_HEIGHT 600
#define GROUND GAME_WINDOW_HEIGHT - GAME_WINDOW_HEIGHT /10

typedef struct Bounds{
    int w;
    int h;
}Bounds;


typedef enum {
    LEFT     = 0x0000001,
    RIGHT    = 0x0000010,
    CENTER_X = 0x0000100,
    CENTER_Y = 0x0001000,
    CENTER   = 0x0010000,
    TOP      = 0x0100000,
    BOTTOM   = 0x1000000
}TextLayout;

extern SDL_Renderer* renderer;

int CreateGameWindow(SDL_Window* window);
int WindowEvents(Dino* dino);
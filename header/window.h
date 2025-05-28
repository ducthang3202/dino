#pragma once
#include <SDL3/SDL.h>
#include <stdio.h>
#include "dino_rush.h"

#define FPS 120
#define PI M_PI
#define GAME_WINDOW_EVENT_NOERR 0
#define GAME_WINDOW_EVENT_EXIT_SAFE 1
#define GAME_WINDOW_EVENT_EXIT_CRASH -1

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
extern SDL_Window* window;
extern bool done;
extern Bounds w_bounds;
extern bool run_title_screen;

int CreateGameWindow();
int WindowEvents();
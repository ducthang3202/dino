#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 400

#define DINO_WIDTH 30
#define DINO_HEIGHT 50
#define GROUND_LEVEL (SCREEN_HEIGHT - DINO_HEIGHT - 50)

#define OBSTACLE_WIDTH 30
#define OBSTACLE_HEIGHT 60

#define MAX_OBSTACLES 3

typedef enum {
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Dino Game - SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("/System/Library/Fonts/NewYork.ttf", 24);
    if (!font) font = TTF_OpenFont("/Library/Fonts/NewYork.ttf", 24);
    if (!font) font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        printf("Warning: Could not load font! SDL_ttf Error: %s\n", TTF_GetError());
    }

    srand(time(NULL));

    GameState game_state = STATE_PLAYING;

    int dino_x = 50;
    float dino_y = GROUND_LEVEL;
    float velocity_y = 0;
    bool is_jumping = false;
    int jump_count = 0;            // Für Doppelsprung
    float gravity = 0.5;

    int obstacle_x[MAX_OBSTACLES];
    int obstacle_y[MAX_OBSTACLES];

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacle_x[i] = SCREEN_WIDTH + i * 300;
        obstacle_y[i] = GROUND_LEVEL + DINO_HEIGHT - OBSTACLE_HEIGHT;
    }

    int score = 0;
    Uint32 score_update_time = SDL_GetTicks();

    int obstacle_speed = 5;
    int next_speedup_score = 50;

    bool is_running = true;
    SDL_Event event;

    while (is_running) {
        Uint32 current_time = SDL_GetTicks();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                if (game_state == STATE_PLAYING && event.key.keysym.sym == SDLK_SPACE) {
                    if (jump_count < 2) {  // Doppelsprung erlauben
                        velocity_y = -10;
                        is_jumping = true;
                        jump_count++;
                    }
                } else if (game_state == STATE_GAME_OVER && event.key.keysym.sym == SDLK_RETURN) {
                    // Reset beim Neustart mit Enter
                    dino_y = GROUND_LEVEL;
                    velocity_y = 0;
                    is_jumping = false;
                    jump_count = 0;
                    score = 0;
                    obstacle_speed = 5;
                    next_speedup_score = 50;
                    score_update_time = SDL_GetTicks();
                    for (int i = 0; i < MAX_OBSTACLES; i++) {
                        obstacle_x[i] = SCREEN_WIDTH + i * 300;
                    }
                    game_state = STATE_PLAYING;
                }
            }
        }

        if (game_state == STATE_PLAYING) {
            // Dino springen & Gravitation
            if (is_jumping) {
                dino_y += velocity_y;
                velocity_y += gravity;
                if (dino_y >= GROUND_LEVEL) {
                    dino_y = GROUND_LEVEL;
                    is_jumping = false;
                    velocity_y = 0;
                    jump_count = 0;
                }
            }

            // Hindernisse bewegen
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                obstacle_x[i] -= obstacle_speed;
                if (obstacle_x[i] + OBSTACLE_WIDTH < 0) {
                    obstacle_x[i] = SCREEN_WIDTH + rand() % 300;
                    score += 1;
                }
            }

            // Score jede Sekunde erhöhen
            if (current_time - score_update_time > 1000) {
                score += 1;
                score_update_time = current_time;
            }

            // Geschwindigkeit alle 50 Punkte erhöhen
            if (score >= next_speedup_score) {
                obstacle_speed += 1;
                next_speedup_score += 50;
            }

            // Kollision prüfen
            SDL_Rect dino_rect = {dino_x, (int)dino_y, DINO_WIDTH, DINO_HEIGHT};
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                SDL_Rect obstacle_rect = {obstacle_x[i], obstacle_y[i], OBSTACLE_WIDTH, OBSTACLE_HEIGHT};
                if (SDL_HasIntersection(&dino_rect, &obstacle_rect)) {
                    game_state = STATE_GAME_OVER;
                }
            }
        }

        // Zeichnen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Score & Speed anzeigen
        if (font) {
            SDL_Color color = {0, 0, 0, 255};
            char text[64];

            sprintf(text, "Score: %d", score);
            SDL_Surface* score_surf = TTF_RenderText_Solid(font, text, color);
            SDL_Texture* score_tex = SDL_CreateTextureFromSurface(renderer, score_surf);
            SDL_Rect score_rect = {SCREEN_WIDTH - score_surf->w - 20, 20, score_surf->w, score_surf->h};
            SDL_RenderCopy(renderer, score_tex, NULL, &score_rect);
            SDL_FreeSurface(score_surf);
            SDL_DestroyTexture(score_tex);

            sprintf(text, "Speed: %d", obstacle_speed);
            SDL_Surface* speed_surf = TTF_RenderText_Solid(font, text, color);
            SDL_Texture* speed_tex = SDL_CreateTextureFromSurface(renderer, speed_surf);
            SDL_Rect speed_rect = {20, 20, speed_surf->w, speed_surf->h};
            SDL_RenderCopy(renderer, speed_tex, NULL, &speed_rect);
            SDL_FreeSurface(speed_surf);
            SDL_DestroyTexture(speed_tex);

            if (game_state == STATE_GAME_OVER) {
                const char* msg = "Game Over! Press ENTER to Restart";
                SDL_Surface* msg_surf = TTF_RenderText_Solid(font, msg, color);
                SDL_Texture* msg_tex = SDL_CreateTextureFromSurface(renderer, msg_surf);
                SDL_Rect msg_rect = {SCREEN_WIDTH / 2 - msg_surf->w / 2, SCREEN_HEIGHT / 2 - msg_surf->h / 2, msg_surf->w, msg_surf->h};
                SDL_RenderCopy(renderer, msg_tex, NULL, &msg_rect);
                SDL_FreeSurface(msg_surf);
                SDL_DestroyTexture(msg_tex);
            }
        }

        // Dino zeichnen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect dino_rect = {dino_x, (int)dino_y, DINO_WIDTH, DINO_HEIGHT};
        SDL_RenderFillRect(renderer, &dino_rect);

        // Hindernisse zeichnen
        SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            SDL_Rect obstacle_rect = {obstacle_x[i], obstacle_y[i], OBSTACLE_WIDTH, OBSTACLE_HEIGHT};
            SDL_RenderFillRect(renderer, &obstacle_rect);
        }

        // Bodenlinie
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderDrawLine(renderer, 0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, SCREEN_HEIGHT - 50);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

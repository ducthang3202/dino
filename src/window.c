#include "../header/window.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h> // for IMG_Load

SDL_Renderer* renderer = NULL;

int CreateGameWindow(SDL_Window** window) {
    printf("Debug: Starting SDL_Init\n");
    printf("Attempting to initialize SDL3...\n");
    
    int flags = SDL_INIT_VIDEO;
    const char* error_before = SDL_GetError();
    printf("SDL_GetError before SDL_Init: %s\n", error_before);
    
    // SDL3 returns true (non-zero) on success, not 0
    bool init_result = SDL_Init(flags);
    if (!init_result) {
        const char* error_after = SDL_GetError();
        printf("SDL3 initialization failed: %s\n", error_after);
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not initialize SDL3: %s\n", error_after);
        return -1;
    }
    printf("SDL3 initialized successfully.\n");
    
    // Create window with centered position and desired size
    printf("Creating window: %dx%d\n", GAME_WINDOW_WIDTH, GAME_WINDOW_HEIGHT);
    *window = SDL_CreateWindow(
        "DinoRush",
        GAME_WINDOW_WIDTH,
        GAME_WINDOW_HEIGHT,
        0
    );
    
    if (*window == NULL) {
        const char* error = SDL_GetError();
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", error);
        printf("Window creation failed: %s\n", error);
        SDL_Quit();
        return -1;
    }
    printf("Window created successfully.\n");
    
    // Load and set window icon (optional)
    SDL_Surface* icon = IMG_Load("assets/DR_Icon.png");
    if (icon) {
        SDL_SetWindowIcon(*window, icon);
        SDL_DestroySurface(icon);
        printf("Window icon set successfully.\n");
    } else {
        printf("Could not load icon: %s\n", SDL_GetError());
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Could not load icon: %s\n", SDL_GetError());
    }
    
    // Create renderer associated with window
    printf("Creating renderer...\n");
    renderer = SDL_CreateRenderer(*window, NULL);
    if (!renderer) {
        const char* error = SDL_GetError();
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer could not be created: %s\n", error);
        printf("Renderer creation failed: %s\n", error);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }
    printf("Renderer created successfully.\n");
    
    return 0;
}

int WindowEvents(Dino* dino) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
            return GAME_WINDOW_EVENT_EXIT_SAFE;
        
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && dino->dead)
            return GAME_WINDOW_EVENT_NOERR;
        
        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat) {
            // SDL3 uses 'key' instead of 'keysym.sym'
            if (event.key.key == SDLK_H) {
                if ((SDL_GetModState() & SDL_KMOD_CTRL) != 0) {
                    show_hitbox = !show_hitbox;
                }
            }
        }
    }
    
    const bool* state = SDL_GetKeyboardState(NULL);
    
    if (dino->dead)
        return GAME_WINDOW_EVENT_NOERR;
    
    if (!dino->mid_air)
        dino->ducking = state[SDL_SCANCODE_S];
    else
        dino->ducking = false;
    
    if (dino->ducking)
        return GAME_WINDOW_EVENT_NOERR;
    
    // Jump logic
    static int jump_timer = 0;
    static bool jump_down = false;
    
    if (state[SDL_SCANCODE_SPACE]) {
        jump_down = true;
        jump_timer++;
        if (jump_timer > actual_fps * 0.12f) {
            dino->long_jump = true;
            if (dino->jump_angle == 0)
                dino->jump_angle += PI / actual_fps;
        }
    } else {
        if (jump_down && dino->jump_angle == 0)
            dino->jump_angle += PI / actual_fps;
        jump_down = false;
        jump_timer = 0;
    }
    
    return GAME_WINDOW_EVENT_NOERR;
}
#include "../header/window.h"

SDL_Renderer* renderer = NULL;


int CreateGameWindow(SDL_Window* window){

    if(!SDL_Init(SDL_INIT_VIDEO)){

        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "- Could not initialize SDL3: %s\n", SDL_GetError());
        return -1;
    }            

    window = SDL_CreateWindow(
        "DinoRush",                         // window title
        GAME_WINDOW_WIDTH,                               // width, in pixels
        GAME_WINDOW_HEIGHT,                                // height, in pixels
        SDL_WINDOW_VULKAN                   // renderer
    );

    SDL_SetWindowIcon(window, IMG_Load("assets/DR_Icon.png") );

    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {

        SDL_Log("Renderer could not be created! SDL_Error: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }


    return 0;
}

int WindowEvents(Dino* dino){
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) 
            return GAME_WINDOW_EVENT_EXIT_SAFE;
        
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && dino->dead)
            return GAME_WINDOW_EVENT_NOERR;

        if (event.key.down && !event.key.repeat) {
            if(event.key.key == SDLK_H )
                if (event.key.key & (SDLK_LCTRL)) 
                    show_hitbox = !show_hitbox;

        }
            
            
    }
    const bool* state = SDL_GetKeyboardState(NULL);

    if(dino->dead)
        return GAME_WINDOW_EVENT_NOERR;



    if(!dino->mid_air)
        dino->ducking = state[SDL_SCANCODE_S];
    else
        dino->ducking = false;
        //SDL_Log("%d\n", dino->ducking);
    if(dino->ducking)
        return GAME_WINDOW_EVENT_NOERR;

    // either force to long jump after holding jump or low jump by tapping space
    // handling keyboard events manually outside of window event loop for faster response
    // PI/(float)FPS -> smooth animation. increment angle, so that jump animation lasts aprox. one second
    
    static int jump_timer = 0;
    static bool jump_down = false;
    if (state[SDL_SCANCODE_SPACE]) {

        jump_down = true; 
        
        jump_timer++;
        if(jump_timer > actual_fps * 0.12f){
            dino->long_jump = true;
            if(dino->jump_angle == 0)
                dino->jump_angle += PI/actual_fps;
        }
    }else{
        if(jump_down && dino->jump_angle == 0)
            dino->jump_angle += PI/actual_fps;

        jump_down = false;
        jump_timer = 0;
    }

    
    return GAME_WINDOW_EVENT_NOERR;
}
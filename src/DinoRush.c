#include "../header/DinoRush.h" 

#define FPS 120.0f

static SDL_Window* window;
static bool done = 0;

int DR_Start(){
    
    if(CreateGameWindow(window) != 0){
        return 1;
    }

    DR_Run();
}

int DR_Run(){
  
    DR_GameLoop();
    // Close and destroy the window
    SDL_DestroyWindow(window);
    // Clean up
    SDL_Quit();
}


int DR_GameLoop(){

    ULONGLONG prev_time = millis();

    // divide one second into FPS intervals
    float interval = 1000.f / 120.f;
    int fps_counter = 0;
    
    while (!done) {
        
        DR_GameLogic();
        DR_WindowEvents();

        DR_FPSCounter(&fps_counter);
        DR_LoopThrottle(&prev_time, interval);
    }
}

void DR_FPSCounter(int* fps_counter){
    
    (*fps_counter)++;

    if(*fps_counter >= FPS){

        printf("FPS: %d\n", *fps_counter);
        *fps_counter = 0;
    }
        
}

void DR_LoopThrottle(ULONGLONG* prev_time, float interval){

    ULONGLONG time = millis();
    ULONGLONG time_passed = time - *prev_time;

    //if loop cycle went too fast for one interval -> sleep for the remainder time
    if(time_passed < interval){
        
        Sleep(interval - time_passed);
    }
    *prev_time = time;
}

void DR_GameLogic(){

}
void DR_WindowEvents(){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            done = true;
        }
    }
}
//wrapper - Windows only
ULONGLONG millis(){
    return GetTickCount64();
}
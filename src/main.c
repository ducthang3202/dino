#include "../header/dino_rush.h"

int main(){

    if(DESIRED_FPS > 700){
        printf("- cant run the game if fps is greater than 700\n");
        Sleep(5000);
        return -1;
    }
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if( CreateGameWindow(window) != 0){
        return -1;
    }
    if(DR_Init() != 0){
        SDL_Log("- couldnt init Game!\n");
        return -1;
    }

    int feedback = DR_TitleScreen();

    if(feedback == -1){
        SDL_Log("- Titlescreen crashed!\n");
        return feedback;
    }
    if(feedback == 1){
        SDL_Log("+ closed Game!\n");
        return feedback;
    }

    feedback = DR_Start();
    while(feedback == 0){
        feedback = DR_Start();

        if(feedback == 1){
            // window closed
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();

    SDL_Log("+ destroyed window/renderer\n");
    SDL_Log("+ quit SDL3\n");
    return 0;
}
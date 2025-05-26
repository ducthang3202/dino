#include "../header/dino_rush.h" 

Dino* dino = NULL;
FILE* high_score_file = NULL;
SDL_Texture* sprites_txt = NULL;
SDL_Texture* bg_txt = NULL;
SDL_Surface* bg_s = NULL;
SDL_Surface* sprites_s = NULL;

bool done = false;
bool dead = false;
bool start_animation = false;

int retry_animation_counter = 0;
int retry_animation = 0;
int high_score = 0, total_high_score = 0;
bool show_hitbox = false;
bool long_jump = false;

int DR_Init(){
    // store player on mem heap
    dino = malloc(sizeof(Dino));

    if(!TTF_Init()){
        SDL_Log("- could not init SDL3_TTF lib!\n");
        return -1;
    }
    if(dino == NULL){
        SDL_Log("- could not allocate memory for Player!\n");
        return -1;
    }
    ground = w_bounds.h - w_bounds.h / 10;
    dino->mid_air = false;
    dino->long_jump = false;
    dino->ducking = false;
    dino->reward = 0;
    dino->hitbox = (SDL_FRect){ w_bounds.w / 8, ground - DINO_H, DINO_W, DINO_H};

    bg_s = IMG_Load("assets/desert_pixel.bmp");
    if(bg_s == NULL){
        SDL_Log("- IMG_Load failed: %s", SDL_GetError());
        return -1;
    }

    sprites_s = IMG_Load("assets/base64.bmp");
    if(sprites_s == NULL){
        SDL_Log("- IMG_Load failed: %s", SDL_GetError());
        return -1;
    }

    bg_txt = SDL_CreateTextureFromSurface(renderer, bg_s);
    sprites_txt = SDL_CreateTextureFromSurface(renderer, sprites_s);
    Level_Init();

    return 0;
}

int DR_Start(){

    done = false;
    dead = false;
    int feedback = 0;

    if(DR_Init() != 0){
        SDL_Log("- couldnt init Game!\n");
        return -1;
    }

    fopen_s(&high_score_file, "high_score.txt", "r");

    if(high_score_file == NULL){
        SDL_Log("- couldnt open high score file!\n");
        return -1;
    }
    char b[128];
    fgets(b, 100, high_score_file);
    total_high_score = atoi(b);
       //SDL_Log("+ high score %d\n", total_high_score);
    feedback = DR_Run();
    if(feedback == GAME_WINDOW_EVENT_EXIT_CRASH){
        SDL_Log("- Unexpected Game crash!\n");
    } 

    snprintf(b, );
    if(high_score > total_high_score){
        fputs();
    }

    SDL_Log("----------------------------\n");
    SDL_Log("+ Cleanup epilouge\n");
    SDL_Log("----------------------------\n\n");

    if(!LL_Clean(obj_list)){
        SDL_Log("- couldnt properly free Linked List data structure\n");
    }else{
        SDL_Log("+ freed Linked List data structure\n");
    }

    free(dino);
    free(obj_list);
    SDL_Log("+ freed allocated structs\n");

    SDL_DestroyTexture(bg_txt);
    SDL_DestroySurface(bg_s);

    SDL_DestroyTexture(sprites_txt);
    SDL_DestroySurface(sprites_s);
    SDL_Log("+ cleaned textures/surfaces\n");

    TTF_Quit();

    SDL_Log("+ disposed SDL3_TTF\n");

    high_score = 0;

    return feedback;
}

int DR_Run(){
  
    int feedback = DR_GameLoop();
    if(feedback == GAME_WINDOW_EVENT_EXIT_CRASH){
        SDL_Log("- something went wrong inside GameLoop!\n");
        return GAME_WINDOW_EVENT_EXIT_CRASH;
    }

    return feedback;
}


int DR_GameLoop(){

    ULONGLONG prev_time = millis();

    // divide one second into FPS intervals
    float interval = 1000.f / (float)FPS;
    int window_feedback = 0;

    while (!done) {
        
        DR_GameLogic();
        window_feedback = WindowEvents();

        DR_LoopThrottle(&prev_time, interval);
    }

    return window_feedback;
}



void DR_LoopThrottle(ULONGLONG* prev_time, float interval){

    ULONGLONG time = millis();
    ULONGLONG time_passed = time - *prev_time;

    //if loop cycle went too fast for one interval -> sleep for the remainder time
    if(time_passed < interval)
        SDL_Delay(interval - time_passed);
    
    *prev_time = time;
}

void DR_GameLogic(){

    if(!dead){
        Dino_Move();
        dead = Level_Update();
    }

    DR_Draw();
}

void DR_DrawText(int x, int y, bool left_centered, char* text_buff, SDL_Color col, TTF_Font* font, SDL_Surface** text_sur){

    *text_sur = TTF_RenderText_Solid(font, text_buff, 0, col);
    
    if (*text_sur) {
        int padding = 10;
        int current_text_width = ((*text_sur)->w + padding);
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, *text_sur);
        SDL_FRect dst = {x - (left_centered ? -1*padding*2 : current_text_width), padding, (*text_sur)->w, (*text_sur)->h}; 
        SDL_RenderTexture(renderer, text_texture, NULL, &dst); 
        SDL_DestroyTexture(text_texture);
    }

}

void DR_Draw(){

    // background
    SDL_RenderTexture(renderer, bg_txt, NULL, NULL);
    TTF_Font* font = TTF_OpenFont("assets/slkscr.ttf", 40);
    if (!font) {
        SDL_Log("Failed to load font slkscr.ttf: %s", SDL_GetError());
        return;
    }
    
    //SDL_Surface* text_sur = NULL;
    char text_buff[128];
    SDL_Surface* text_sur = NULL;
    // draw FPS:
    SDL_Color black = (SDL_Color) {0, 0, 0};
    snprintf(text_buff, sizeof(text_buff), "FPS: %d", FPS);
    DR_DrawText(0, 0,true, text_buff,black, font, &text_sur);
    SDL_DestroySurface(text_sur);

    // re-use text buffer
    // draw HI:
    snprintf(text_buff, sizeof(text_buff), "HI: %d", high_score);
    DR_DrawText(w_bounds.w, 0, false, text_buff,black,font, &text_sur);
    int text_width = w_bounds.w - text_sur->w - 20;
    SDL_DestroySurface(text_sur);

    SDL_Color red = (SDL_Color) {0xff, 0x33, 0x00};
    snprintf(text_buff, sizeof(text_buff), "%d", total_high_score);
    DR_DrawText(text_width,0,false,text_buff,red,font, &text_sur);
    text_width -=  text_sur->w ;
    SDL_DestroySurface(text_sur);

    snprintf(text_buff, sizeof(text_buff), "TOTAL HI: ");
    DR_DrawText(text_width,0,false,text_buff,black,font, &text_sur);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    Dino_Draw();

    if(obj_list->size > 0){
        Node* curr = obj_list->NIL;
        while((curr = curr->next) != obj_list->NIL){
            ((Obstacle*)curr->data)->draw((Obstacle*)curr->data);
        }
    }
    
    // if dead render all current entities with dead screen on top
    if(dead){
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 128, 128, 128, 50);
        SDL_FRect screen = (SDL_FRect){0,0,w_bounds.w, w_bounds.h};
        SDL_RenderFillRect(renderer, &screen);

        retry_animation_counter++;
        if(retry_animation_counter > FPS / 3){
            retry_animation = (retry_animation + 1) % 4;
            retry_animation_counter = 0;
        }
            
        SDL_FRect retry_r_src = { 145 + retry_animation * 72, 130 , 72 , 70 };
        SDL_FRect retry_r = { w_bounds.w / 2 - 50 , w_bounds.h / 2 - 50, 72, 70 };
        SDL_RenderTexture(renderer, sprites_txt, &retry_r_src, &retry_r);

       // SDL_RenderRect(renderer,& retry_r);
    }
    // present buffer to screen
    SDL_RenderPresent(renderer);
    SDL_DestroySurface(text_sur);
    
    TTF_CloseFont(font);
}



//wrapper
ULONGLONG millis(){
#ifdef WIN
    return GetTickCount64();
#endif
#ifdef MAC
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); 
    return ts.tv_nsec/1000000;
#endif
#ifdef LINUX
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); 
    return ts.tv_nsec/1000000;
#endif

}
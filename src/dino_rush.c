#include "../header/dino_rush.h" 

Dino* dino = NULL;
SDL_Texture* sprites_txt = NULL;
SDL_Texture* bg_txt = NULL;
TTF_Font* font = NULL;

bool done = false;
bool dead = false;

int retry_animation_counter = 0;
int retry_animation = 0;
int high_score = 0, total_high_score = 0;
bool show_hitbox = false;
bool long_jump = false;
bool run_title_screen = true;

int DR_Run();
int DR_GameLoop();
int DR_Init();
Bounds DR_DrawText(int x, int y, TextLayout layout, SDL_Color col, const char* text_fmt, ... );
void DR_LoopThrottle(ULONGLONG* prev_time, float interval);
void DR_GameLogic();
void DR_Draw();
void DR_RenderText(int x, int y, SDL_Color col, TextLayout layout, SDL_Surface* text_sur);

int DR_TitleScreen(){

    TTF_SetFontSize(font, 80);

    ULONGLONG prev_time = millis();
    // divide one second into FPS intervals
    float interval = 1000.f / (float)FPS;
    int feedback = 0, flashing_animation_counter = 0, y_pos = 0;
    float alpha = 0;

    while(run_title_screen){
        // regulation of title animation
        // 1.fading 
        // 2.moving to the top
        // 3.flashing
        if(alpha < 255.f && flashing_animation_counter == 0)
            alpha += 0.5f;
        else if(w_bounds.h/2 + y_pos > w_bounds.h / 8 && flashing_animation_counter == 0){
            alpha = 255.f;
            y_pos--;
        }
        else if(flashing_animation_counter < FPS){
            flashing_animation_counter++;
            
        }else{
            // -1 to correct the sign bit
            // float is casted to Uint8
            // ~ treats operand(alpha) as 32 signed int 
            // then all 32 bits are inverted. 0xffffff00
            // 0xffffff00 is not positive -> the most significant bit is 1, which marks it as a negative number in twos complement.
            // 0xffffff00 -> 0x000000ff -> +1 -> -0x00000100 = -256
            // to prevent overflow from + 1 from 255 -> 256 subtract 1 from alpha in the beginning
            
            alpha = ~(Uint8)(alpha-1);
            flashing_animation_counter = 1;
        }
        

        printf("%f %d\n", alpha, flashing_animation_counter);
        SDL_RenderTexture(renderer, bg_txt, NULL, NULL);

        DR_DrawText(w_bounds.w/2, w_bounds.h/2 + y_pos, RIGHT | CENTER_Y, (SDL_Color) {0xff, 0x33, 0x00, (Uint8)alpha}, "Dino");
        DR_DrawText(w_bounds.w/2, w_bounds.h/2 + y_pos, LEFT | CENTER_Y, (SDL_Color) {255, 0, 0, (Uint8)alpha}, "Rush");

        SDL_RenderPresent(renderer);

        feedback = WindowEvents();
        DR_LoopThrottle(&prev_time, interval);
        
    }

    TTF_SetFontSize(font, 40);

    return feedback;
}

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
   
    font = TTF_OpenFont("assets/slkscr.ttf", 40);
    if (!font) {
        SDL_Log("- Failed to load font slkscr.ttf: %s", SDL_GetError());
        return -1;
    }

    // sprites init
    SDL_Surface* bg_s = IMG_Load("assets/desert_pixel.bmp");
    if(bg_s == NULL){
        SDL_Log("- IMG_Load failed: %s", SDL_GetError());
        return -1;
    }
    SDL_Surface* sprites_s = IMG_Load("assets/base64.bmp");
    if(sprites_s == NULL){
        SDL_Log("- IMG_Load failed: %s", SDL_GetError());
        return -1;
    }
    bg_txt = SDL_CreateTextureFromSurface(renderer, bg_s);
    sprites_txt = SDL_CreateTextureFromSurface(renderer, sprites_s);

    SDL_DestroySurface(bg_s);
    SDL_DestroySurface(sprites_s);

    ground = w_bounds.h - w_bounds.h / 10;
    dino->mid_air = false;
    dino->long_jump = false;
    dino->ducking = false;
    dino->reward = 0;
    dino->hitbox = (SDL_FRect){ w_bounds.w / 8, ground - DINO_H, DINO_W, DINO_H};

    Level_Init();

    return 0;
}

int DR_Start(){

    done = false;
    dead = false;
    int feedback = 0;
    high_score = 0;

    if(DR_Init() != 0){
        SDL_Log("- couldnt init Game!\n");
        return -1;
    }
    feedback = DR_TitleScreen();

    if(feedback != 0){
        SDL_Log("- couldnt init Game!\n");
        return feedback;
    }


    // high score file init
    FILE* high_score_file = NULL;

    fopen_s(&high_score_file, "high_score.txt", "r");

    if(high_score_file == NULL){
        SDL_Log("- couldnt open high score file to read high score!\n");
        return -1;
    }

    char b[128];
    fgets(b, 100, high_score_file);
    total_high_score = atoi(b);
    fclose(high_score_file);
       //SDL_Log("+ high score %d\n", total_high_score);
    feedback = DR_Run();
    if(feedback == GAME_WINDOW_EVENT_EXIT_CRASH){
        SDL_Log("- Unexpected Game crash!\n");
    } 

    if(high_score > total_high_score){

        fopen_s(&high_score_file, "high_score.txt", "w");

         if(high_score_file == NULL){
            SDL_Log("- couldnt open high score file to store high score!\n");
            return -1;
        }
        // over write existing buffer
        snprintf(b, 100, "%d", high_score);
        fputs(b, high_score_file);
        fclose(high_score_file);
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
    SDL_DestroyTexture(sprites_txt);

    SDL_Log("+ cleaned textures/surfaces\n");

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_Log("+ disposed SDL3_TTF\n");

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

void DR_RenderText(int x, int y, SDL_Color col, TextLayout layout, SDL_Surface* text_sur){

    int padding = 10, x_adjust = 0, y_adjust = 0;
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_sur);

    if(layout & TOP)
        y_adjust = -text_sur->h;
    if(layout & BOTTOM)
        y_adjust = text_sur->h;

    if(layout & RIGHT)
        x_adjust = -text_sur->w;

    if(layout & CENTER){
        x_adjust = -text_sur->w/2;
        y_adjust = -text_sur->h/2;
    }
    if(layout & CENTER_Y)
        y_adjust = -text_sur->h/2;
    if(layout & CENTER_X)
        x_adjust = -text_sur->w/2;
    

    SDL_FRect dst = {x + x_adjust + padding, y + y_adjust + padding, text_sur->w, text_sur->h}; 
    SDL_RenderTexture(renderer, text_texture, NULL, &dst); 
    SDL_DestroyTexture(text_texture);
}

Bounds DR_DrawText(int x, int y, TextLayout layout, SDL_Color col, const char* text_fmt, ... ){

    char buff[128];

    if(sizeof(text_fmt) > 100)
        return (Bounds){0, 0}; 

    va_list args;
    va_start(args, text_fmt);            
    vsnprintf(buff, sizeof(buff), text_fmt, args);
    va_end(args);                      

    SDL_Surface* text_sur = TTF_RenderText_Blended(font, buff, 0, col);
    
    if (!text_sur) 
        return (Bounds){0, 0}; 
    
    DR_RenderText(x, y, col, layout, text_sur);

    Bounds b = (Bounds){text_sur->w, text_sur->h};
    SDL_DestroySurface(text_sur);

    return b;
}

void DR_Draw(){

    // background
    SDL_RenderTexture(renderer, bg_txt, NULL, NULL);

    //SDL_Surface* text_sur = NULL;
    char text_buff[128];
    SDL_Surface* text_sur = NULL;
    // draw FPS:
    /*
    SDL_Color black = (SDL_Color) {0, 0, 0};
    snprintf(text_buff, sizeof(text_buff), "FPS: %d", FPS);
    DR_DrawText(0, 0, LEFT, text_buff,black, &text_sur);
    SDL_DestroySurface(text_sur);

    //DR_DrawText(0, 0, LEFT, (SDL_Color) {0, 0, 0}, "FPS: %d");
    // re-use text buffer
    // draw HI:
    snprintf(text_buff, sizeof(text_buff), "HI: %d", high_score);
    DR_DrawText(w_bounds.w, 0, RIGHT, text_buff,black, &text_sur);
    int text_width = w_bounds.w - text_sur->w - 20;
    SDL_DestroySurface(text_sur);

    SDL_Color red = (SDL_Color) {0xff, 0x33, 0x00};
    snprintf(text_buff, sizeof(text_buff), "%d", total_high_score);
    DR_DrawText(text_width, 0, RIGHT,text_buff,red, &text_sur);
    text_width -=  text_sur->w ;
    SDL_DestroySurface(text_sur);

    snprintf(text_buff, sizeof(text_buff), "TOTAL HI: ");
    DR_DrawText(text_width, 0, RIGHT,text_buff,black, &text_sur);
*/
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
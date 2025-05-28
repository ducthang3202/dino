#include "../header/level.h"

LinkedList* obj_list = NULL;
bool timer_running = false;
int timer = 0;
int rand_air_spawn = 0;
int delay = 0;
float ground = 0;

void Level_Init(){
    
    obj_list = LL_Create();

    // seed is not neccessary
    srand(time(NULL));
}

void Level_StartTimer(){
    delay = r(1500,3000);
    timer = millis();
    timer_running = true;
}

bool Level_Update(){

    bool end = false;
    
    if(!timer_running)
        Level_StartTimer();
    
    if(timer_running && millis() - timer > delay){
        timer_running = false;

        if(r(1,3) == 2)
            LL_Add(obj_list, Level_CreatePterodactyl());
        else
            LL_Add(obj_list, Level_CreateCactus());
    } 
    
    if(obj_list->size == 0)
        return end;

    // main obstacle list iteration. Most actions between dino and obstacle will be handled in here
    Node* curr = obj_list->NIL;
    while((curr = curr->next) != obj_list->NIL){
        Obstacle* obs = (Obstacle*)curr->data;
        obs->move(obs);

        if(Level_DetectCollision(obs)){
            end = true;
            break;
        }
        if(dino->mid_air && dino->reward == 0){
            Dino_PassedObstacle(obs);
        }
        if(obs->hitbox.x < -obs->hitbox.w)
            Level_RemoveObstacle(obs);
    }

    return end;
}

bool Level_DetectCollision(Obstacle* obs){

    bool right = dino->hitbox.x + dino->hitbox.w - GRACE_ZONE > obs->hitbox.x && dino->hitbox.x + dino->hitbox.w - GRACE_ZONE < obs->hitbox.x + obs->hitbox.w;
    bool bottom = dino->hitbox.y + dino->hitbox.h - GRACE_ZONE > obs->hitbox.y && dino->hitbox.y + dino->hitbox.h - GRACE_ZONE < obs->hitbox.y + obs->hitbox.h;
    bool top = dino->hitbox.y + GRACE_ZONE/2 > obs->hitbox.y && dino->hitbox.y + GRACE_ZONE/2 < obs->hitbox.y + obs->hitbox.h;
    bool left = dino->hitbox.x + GRACE_ZONE > obs->hitbox.x && dino->hitbox.x + GRACE_ZONE < obs->hitbox.x + obs->hitbox.w;

    if((right || left) && (top || bottom) ){
        SDL_Log("collision %d right: %d bottom: %d top: %d dino top: %f obstop: %f left: %d\n", obs->type, right, bottom, top,dino->hitbox.y+GRACE_ZONE, obs->hitbox.y, left);
        return true;
    }
    return false;
}

Obstacle* Level_CreateCactus(){
    Obstacle* obs = malloc(sizeof(Obstacle));
    if (obs == NULL) 
        return NULL; 

    obs->hitbox = (SDL_FRect){w_bounds.w, ground - CACTUS_H , CACTUS_W, CACTUS_H};
    obs->move = Obstacle_MoveCactus;
    obs->draw = Obstacle_DrawCactus;
    obs->type = GROUND;
    obs->sprite = r(0, 4);

    if(obs->sprite == 4){
        obs->hitbox.w = CACTUS_W*2;
    }
   // o->id = (ObstacleID){'c', 0};

    return obs;
}
Obstacle* Level_CreatePterodactyl(){
    Obstacle* obs = malloc(sizeof(Obstacle));
    if (obs == NULL) 
        return NULL; 

    obs->hitbox = (SDL_FRect){ w_bounds.w, ground - DINO_H + 30 - 85 , 100, 85};
    obs->move = Obstacle_MovePterodactyl;
    obs->draw = Obstacle_DrawPterodactyl;
    obs->type = AIR;
    obs->sprite = 0;

    return obs;
}

void Level_RemoveObstacle(Obstacle* obs){
    if(obs != NULL)
        LL_Remove(obj_list, obs);
    
}

int r(int min, int max) {
    return rand() % (max - min + 1) + min;
}
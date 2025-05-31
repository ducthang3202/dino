#include "../header/level.h"


int r(int min, int max) {
    return rand() % (max - min + 1) + min;
}

bool Level_DetectCollision(Dino* dino, Obstacle* obs){

    bool right = dino->go.hitbox.x + dino->go.hitbox.w - GRACE_ZONE > obs->go.hitbox.x && dino->go.hitbox.x + dino->go.hitbox.w - GRACE_ZONE < obs->go.hitbox.x + obs->go.hitbox.w;
    bool bottom = dino->go.hitbox.y + dino->go.hitbox.h - GRACE_ZONE > obs->go.hitbox.y && dino->go.hitbox.y + dino->go.hitbox.h - GRACE_ZONE < obs->go.hitbox.y + obs->go.hitbox.h;
    bool top = dino->go.hitbox.y + GRACE_ZONE/2 > obs->go.hitbox.y && dino->go.hitbox.y + GRACE_ZONE/2 < obs->go.hitbox.y + obs->go.hitbox.h;
    bool left = dino->go.hitbox.x + GRACE_ZONE > obs->go.hitbox.x && dino->go.hitbox.x + GRACE_ZONE < obs->go.hitbox.x + obs->go.hitbox.w;

    if((right || left) && (top || bottom) ){
        SDL_Log("collision %d right: %d bottom: %d top: %d dino top: %f obstop: %f left: %d\n", obs->type, right, bottom, top,dino->go.hitbox.y+GRACE_ZONE, obs->go.hitbox.y, left);
        return true;
    }
    return false;
}

Obstacle* Level_CreateCactus(SDL_Texture* sprites_txt){
    Obstacle* obs = malloc(sizeof(Obstacle));
    if (obs == NULL) 
        return NULL; 

    obs->go.hitbox = (SDL_FRect){GAME_WINDOW_WIDTH, GROUND_HEIGHT - CACTUS_H , CACTUS_W, CACTUS_H};
    obs->go.move = Obstacle_MoveCactus;
    obs->go.draw = Obstacle_DrawCactus;
    obs->go.childclass_ref = obs;
    obs->go.sprites_txt = sprites_txt;
    obs->type = GROUND;
    obs->sprite = r(0, 4);
    obs->animation_counter = 0;


    if(obs->sprite == 4){
        obs->go.hitbox.w = CACTUS_W*2;
    }
   // o->id = (ObstacleID){'c', 0};

    return obs;
}
Obstacle* Level_CreatePterodactyl(SDL_Texture* sprites_txt){
    Obstacle* obs = malloc(sizeof(Obstacle));
    if (obs == NULL) 
        return NULL; 

    obs->go.hitbox = (SDL_FRect){ GAME_WINDOW_WIDTH, GROUND_HEIGHT - DINO_H + 30 - 85 , 100, 85};
    obs->go.move = Obstacle_MovePterodactyl;
    obs->go.draw = Obstacle_DrawPterodactyl;
    obs->go.childclass_ref = obs;
    obs->go.sprites_txt = sprites_txt;
    obs->type = AIR;
    obs->sprite = 0;
    obs->animation_counter = 0;

    return obs;
}

void Level_RemoveObstacle(LinkedList* obj_list, Obstacle* obs){
    if(obs != NULL)
        LL_Remove(obj_list, obs);
    
}

void Level_Update(LinkedList* obj_list, Dino* dino){

    static bool timer_running = false;
    static uint64_t timer = 0;
    static int delay = 0;

    if(!timer_running){
        delay = r(2000,3500);
        timer = millis();
        timer_running = true;
    }
    
    if(timer_running && millis() - timer > delay){
        timer_running = false;

        if(r(1,3) == 2)
            LL_Add(obj_list, Level_CreatePterodactyl(dino->go.sprites_txt));
        else
            LL_Add(obj_list, Level_CreateCactus(dino->go.sprites_txt));
    } 
    
    if(obj_list->size == 0)
        return;

    // main obstacle list iteration. Most actions between dino and obstacle will be handled in here
    Node* curr = obj_list->NIL;
    while((curr = curr->next) != obj_list->NIL){
        Obstacle* obs = (Obstacle*)curr->data;
        obs->go.move(&obs->go);

        if(Level_DetectCollision(dino, obs)){
            dino->dead = true;
            break;
        }
        if(dino->mid_air && dino->reward == 0){
            Dino_PassedObstacle(dino, obs);
        }
        if(obs->go.hitbox.x < -obs->go.hitbox.w)
            Level_RemoveObstacle(obj_list, obs);
    }
}
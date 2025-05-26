#pragma once

#include <time.h>
#include <stdlib.h>

#define MAX_OBJECTS 10

// grace zone for player hitbox collision
#define GRACE_ZONE 28

#include "game_object.h"
#include "ll.h"

extern unsigned long long millis();
extern int r(int min, int max);
extern int obj_count;
extern float ground;

// linked list for quick removal of elements / searching
extern LinkedList* obj_list;

void Level_Init();
bool Level_Update();
bool Level_DetectCollision(Obstacle* obs);
Obstacle* Level_CreateCactus();
Obstacle* Level_CreatePterodactyl();
void Level_RemoveObstacle(Obstacle* o);
int r(int min, int max);
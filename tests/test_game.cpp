#include <gtest/gtest.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>

extern "C" {
    #include "../header/dino_rush.h"
    #include "../header/game_object.h"
    #include "../header/level.h"
    #include "../header/window.h"
    #include "../header/ll.h"
}

// Test LinkedList operations
TEST(LinkedListTest, CreateAndDestroy) {
    LinkedList* list = LL_Create();
    ASSERT_NE(list, nullptr);
    EXPECT_EQ(list->size, 0);
    
    LL_Clean(list);
    free(list);
}

TEST(LinkedListTest, AddAndRemove) {
    LinkedList* list = LL_Create();
    
    // Add some data
    int* data1 = (int*)malloc(sizeof(int));
    *data1 = 42;
    LL_Add(list, data1);
    EXPECT_EQ(list->size, 1);
    
    int* data2 = (int*)malloc(sizeof(int));
    *data2 = 100;
    LL_Add(list, data2);
    EXPECT_EQ(list->size, 2);
    
    // Remove data
    LL_Remove(list, data1);
    EXPECT_EQ(list->size, 1);
    
    LL_Remove(list, data2);
    EXPECT_EQ(list->size, 0);
    
    LL_Clean(list);
    free(list);
}

// Test Dino basic functionality
TEST(DinoTest, DinoInitialization) {
    Dino dino;
    dino.mid_air = false;
    dino.long_jump = false;
    dino.ducking = false;
    dino.dead = false;
    dino.reward = 0;
    dino.jump_angle = 0;
    
    EXPECT_FALSE(dino.mid_air);
    EXPECT_FALSE(dino.long_jump);
    EXPECT_FALSE(dino.ducking);
    EXPECT_FALSE(dino.dead);
    EXPECT_EQ(dino.reward, 0);
    EXPECT_EQ(dino.jump_angle, 0);
}

TEST(DinoTest, DinoMovement) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Test", 1, 1, SDL_WINDOW_HIDDEN);
    renderer = SDL_CreateRenderer(window, NULL);
    
    Dino dino;
    dino.go.hitbox = (SDL_FRect){100, GROUND_HEIGHT - DINO_H, DINO_W, DINO_H};
    dino.go.move = Dino_Move;
    dino.go.childclass_ref = &dino;
    dino.mid_air = false;
    dino.long_jump = false;
    dino.ducking = false;
    dino.dead = false;
    dino.reward = 0;
    dino.jump_angle = 0;
    
    float initial_y = dino.go.hitbox.y;
    
    // Start a jump
    dino.jump_angle = M_PI / 60; // Small angle to start
    dino.mid_air = true;
    
    // Move once
    Dino_Move(&dino.go);
    
    // Y position should have changed (jumped up)
    EXPECT_NE(dino.go.hitbox.y, initial_y);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = nullptr;
}

// Test Obstacle basic functionality
TEST(ObstacleTest, ObstacleStructure) {
    Obstacle obs;
    obs.type = GROUND;
    obs.sprite = 0;
    obs.animation_counter = 0;
    obs.go.hitbox = (SDL_FRect){1000, 400, 50, 100};
    
    EXPECT_EQ(obs.type, GROUND);
    EXPECT_EQ(obs.sprite, 0);
    EXPECT_EQ(obs.go.hitbox.x, 1000);
}

TEST(ObstacleTest, ObstacleMovement) {
    SDL_Init(SDL_INIT_VIDEO);
    
    Obstacle cactus;
    cactus.go.hitbox = (SDL_FRect){1000, GROUND_HEIGHT - CACTUS_H, CACTUS_W, CACTUS_H};
    cactus.go.move = Obstacle_MoveCactus;
    cactus.go.childclass_ref = &cactus;
    
    float initial_x = cactus.go.hitbox.x;
    actual_fps = 60; // Set a known FPS
    
    // Move the cactus
    cactus.go.move(&cactus.go);
    
    // Should have moved left
    EXPECT_LT(cactus.go.hitbox.x, initial_x);
}

// Test scoring
TEST(GameTest, HighScore) {
    high_score = 0;
    
    // Simulate scoring
    high_score += 10;
    EXPECT_EQ(high_score, 10);
    
    high_score += 20;
    EXPECT_EQ(high_score, 30);
}

// Test window dimensions
TEST(WindowTest, Dimensions) {
    EXPECT_GT(GAME_WINDOW_WIDTH, 0);
    EXPECT_GT(GAME_WINDOW_HEIGHT, 0);
    EXPECT_GT(GROUND_HEIGHT, 0);
    EXPECT_LT(GROUND_HEIGHT, GAME_WINDOW_HEIGHT);
}

// Test game state
TEST(GameTest, GlobalVariables) {
    // Test initial states
    show_hitbox = false;
    EXPECT_FALSE(show_hitbox);
    
    show_hitbox = true;
    EXPECT_TRUE(show_hitbox);
    
    actual_fps = 60.0f;
    EXPECT_EQ(actual_fps, 60.0f);
}

// Simple collision test
TEST(CollisionTest, BasicRectCollision) {
    // Test basic AABB collision
    SDL_FRect rect1 = {100, 100, 50, 50};
    SDL_FRect rect2 = {200, 100, 50, 50};
    
    // No collision when apart
    bool collides = (rect1.x < rect2.x + rect2.w &&
                    rect1.x + rect1.w > rect2.x &&
                    rect1.y < rect2.y + rect2.h &&
                    rect1.y + rect1.h > rect2.y);
    EXPECT_FALSE(collides);
    
    // Collision when overlapping
    rect2.x = 120;
    collides = (rect1.x < rect2.x + rect2.w &&
               rect1.x + rect1.w > rect2.x &&
               rect1.y < rect2.y + rect2.h &&
               rect1.y + rect1.h > rect2.y);
    EXPECT_TRUE(collides);
}

// Test Level_Update exists and can be called
TEST(LevelTest, LevelUpdateCall) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Test", 1, 1, SDL_WINDOW_HIDDEN);
    renderer = SDL_CreateRenderer(window, NULL);
    
    LinkedList* obstacles = LL_Create();
    Dino dino;
    dino.dead = false;
    dino.mid_air = false;
    dino.reward = 0;
    dino.go.hitbox = (SDL_FRect){100, GROUND_HEIGHT - DINO_H, DINO_W, DINO_H};
    dino.go.sprites_txt = nullptr;
    
    // This should not crash
    Level_Update(obstacles, &dino);
    
    // Clean up any spawned obstacles
    while (obstacles->size > 0) {
        Node* node = obstacles->NIL->next;
        if (node != obstacles->NIL) {
            LL_Remove(obstacles, node->data);
        }
    }
    
    LL_Clean(obstacles);
    free(obstacles);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = nullptr;
}

// Main function
int main(int argc, char **argv) {
    // Initialize SDL once for all tests
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    if (!TTF_Init()) {
        std::cerr << "Failed to initialize SDL_ttf: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    // Initialize global variables
    high_score = 0;
    show_hitbox = false;
    actual_fps = 60.0f;
    
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    
    TTF_Quit();
    SDL_Quit();
    
    return result;
}
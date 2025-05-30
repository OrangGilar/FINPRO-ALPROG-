#include "raylib.h"
#include "constants.h"
#include "globals.h"    
#include "game_logic.h" 
#include "ui_drawing.h" 
#include "json_handling.h"  // For loading items
// No direct need for binary_logging.h here as logging calls are in game_logic
#include <ctime>       
#include <cstdlib>     

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Roulette Game - Full Features");
    SetTargetFPS(TARGET_FPS);
    std::srand(static_cast<unsigned int>(time(nullptr))); 

    if (!LoadItemsFromConfig("items_config.json")) {
        // message is set in LoadItemsFromConfig on error.
        // The function loads a fallback item if config loading fails.
        // You might want to display this initial message for a moment or handle it.
    }

    ResetGame(); 

    while (!WindowShouldClose()) {
        UpdateGame(); 
        DrawGameUI(); 
    }

    CloseWindow();
    return 0;
}
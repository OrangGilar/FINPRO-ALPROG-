#include "raylib.h"
#include "constants.h"
#include "globals.h"
#include "logic.h" 
#include "ui.h" 
#include <ctime>       
#include <cstdlib>     

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Russian Roulette - Multi-File NoComm");
    SetTargetFPS(TARGET_FPS);
    std::srand(static_cast<unsigned int>(time(nullptr))); 

    ResetGame(); 

    while (!WindowShouldClose()) {
        UpdateGame(); 
        DrawGameUI(); 
    }

    CloseWindow();
    return 0;
}
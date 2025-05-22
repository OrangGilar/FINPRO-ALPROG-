#include "raylib.h"
#include <vector>
#include <string>
#include <sstream>
#include <ctime>
#include <algorithm>

#include "items.h"
#include "characters.h"
#include "search.h"

std::vector<Item> gameItems = {
    {"Health Pot", "Gives 2 health"},
    {"Knife", "Makes double damage"},
    {"Goggles", "See whether it’s dangerous or not"},
    {"Energy Drink", "Take two turns"}
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Turn-Based Russian Roulette with AI");
    SetTargetFPS(60);

    bool gameOver = false;
    int survivalCount = 0;
    const int chambers = 6;
    std::vector<int> highScores;

    bool playerTurn = rand() % 2 == 0;
    bool aiDead = false;
    bool playerDead = false;

    while (!WindowShouldClose()) {
        if (!gameOver) {
            if (playerTurn && IsKeyPressed(KEY_SPACE)) {
                int bulletPos = rand() % chambers;
                if (bulletPos == 0) {
                    playerDead = true;
                    gameOver = true;
                    highScores.push_back(survivalCount);
                    quickSort(highScores, 0, highScores.size() - 1);
                } else {
                    survivalCount++;
                    playerTurn = false;
                }
            } else if (!playerTurn) {
                static int frameWait = 60;
                if (frameWait-- <= 0) {
                    frameWait = 60;
                    int bulletPos = rand() % chambers;
                    if (bulletPos == 0) {
                        aiDead = true;
                        gameOver = true;
                        highScores.push_back(survivalCount);
                        quickSort(highScores, 0, highScores.size() - 1);
                    } else {
                        survivalCount++;
                        playerTurn = true;
                    }
                }
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                survivalCount = 0;
                playerTurn = rand() % 2 == 0;
                aiDead = false;
                playerDead = false;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameOver) {
            DrawText("Turn-Based Russian Roulette", 20, 10, 20, DARKGRAY);
            DrawText(playerTurn ? "Your Turn: Press [SPACE] to pull the trigger" : "AI's Turn...", 20, 40, 20, playerTurn ? BLUE : MAROON);
            DrawText(TextFormat("Safe pulls: %i", survivalCount), 20, 80, 30, BLACK);

           

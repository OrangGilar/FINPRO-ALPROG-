#include "raylib.h"
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include "items.h"
#include "characters.h"

Character player{"Player", 5};
Character ai{"AI", 5};

std::vector<Item> allItems = {
    {"Health Pot", "Gives 2 health"},
    {"Knife", "Double damage"},
    {"Goggles", "See danger"},
    {"Energy Drink", "Take two turns"}
};

Rectangle shootSelfBtn = {280, 80, 240, 40};
Rectangle getItemBtn = {280, 140, 240, 40};
Rectangle skipTurnBtn = {280, 200, 240, 40};
Rectangle shootEnemyBtn = {280, 260, 240, 40};

bool CheckButton(Rectangle button) {
    return CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void DrawCharacterStatus(Character& c, int x, int y) {
    DrawText(TextFormat("%s HP: %d", c.name.c_str(), c.health), x, y, 20, BLACK);
    DrawText("Items:", x, y + 25, 20, BLACK);
    for (int i = 0; i < c.inventory.size(); i++) {
        DrawText(c.inventory[i].name.c_str(), x + 10, y + 50 + i * 20, 18, DARKGRAY);
    }
}

Item GetRandomItem() {
    return allItems[std::rand() % allItems.size()];
}

int main() {
    InitWindow(800, 600, "Russian Roulette UI");
    SetTargetFPS(60);
    std::srand(time(nullptr));

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw buttons
        DrawRectangleRec(shootSelfBtn, LIGHTGRAY);
        DrawRectangleRec(getItemBtn, LIGHTGRAY);
        DrawRectangleRec(skipTurnBtn, LIGHTGRAY);
        DrawRectangleRec(shootEnemyBtn, LIGHTGRAY);

        DrawText("Shoot Self", shootSelfBtn.x + 60, shootSelfBtn.y + 10, 20, BLACK);
        DrawText("Get Items", getItemBtn.x + 60, getItemBtn.y + 10, 20, BLACK);
        DrawText("Skip Turn", skipTurnBtn.x + 60, skipTurnBtn.y + 10, 20, BLACK);
        DrawText("Shoot Enemy", shootEnemyBtn.x + 60, shootEnemyBtn.y + 10, 20, BLACK);

        // Draw character panels
        DrawCharacterStatus(player, 30, 30);
        DrawCharacterStatus(ai, 30, 400);

        // Button logic
        if (CheckButton(getItemBtn)) {
            Item newItem = GetRandomItem();
            player.inventory.push_back(newItem);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}


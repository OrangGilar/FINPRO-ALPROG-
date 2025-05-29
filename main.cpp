#include "raylib.h"
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm> 
#include "items.h"
#include "characters.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MAX_CHARACTER_HEALTH = 3;
const int CYLINDER_SIZE = 6;
const int MAX_INVENTORY_SIZE = 3;

const std::string ITEM_HEALTH_POT = "Health Pot";
const std::string ITEM_KNIFE = "Knife";
const std::string ITEM_GOGGLES = "Goggles";
const std::string ITEM_ENERGY_DRINK = "Energy Drink";

Character player; 
Character ai;     

std::vector<Item> allGameItems = {
    {ITEM_HEALTH_POT, "Restores 2 health points"},
    {ITEM_KNIFE, "Enables double damage"},
    {ITEM_GOGGLES, "Allows seeing danger"},
    {ITEM_ENERGY_DRINK, "Enables taking two turns"}
};
Rectangle shootSelfBtn = { (float)SCREEN_WIDTH/2 - 120, (float)SCREEN_HEIGHT - 260, 240, 40}; 
Rectangle getItemBtn = { (float)SCREEN_WIDTH/2 - 120, (float)SCREEN_HEIGHT - 140, 240, 40};
Rectangle skipTurnBtn = { (float)SCREEN_WIDTH/2 - 120, (float)SCREEN_HEIGHT - 80, 240, 40};
Rectangle shootEnemyBtn = { (float)SCREEN_WIDTH/2 - 120, (float)SCREEN_HEIGHT - 200, 240, 40}; 

Rectangle useItem1Btn = { 30, 250, 200, 30 };
Rectangle useItem2Btn = { 30, 290, 200, 30 };
Rectangle useItem3Btn = { 30, 330, 200, 30 };

bool playerTurn = true;
bool gameOver = false;
std::string gameStatusMessage = "Welcome! Player's turn.";
std::string lastActionMessage = "Spin the cylinder!";

int bulletChamber = -1; 
int currentChamber = 0; 

bool IsButtonClicked(Rectangle button) {
    return CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}
void UpdatePassiveItemFlags(Character& character) {
    character.hasKnife = false;
    character.hasGoggles = false;

    for (const Item& item : character.inventory) {
        if (item.name == ITEM_KNIFE) {
            character.hasKnife = true;
        } else if (item.name == ITEM_GOGGLES) {
            character.hasGoggles = true;
        }
    }
}
void GiveItemToCharacter(Character& character, const Item& item) {
    if (character.inventory.size() < MAX_INVENTORY_SIZE) {
        character.inventory.push_back(item);
        UpdatePassiveItemFlags(character);
        lastActionMessage = character.name + " received: " + item.name;
    } else {
        lastActionMessage = character.name + "'s inventory is full!";
    }
}
Item GetRandomGameItem() {
    if (allGameItems.empty()) {
        return {"Error Item", "No items defined"};
    }
    return allGameItems[std::rand() % allGameItems.size()];
}
void UseItemFromInventory(Character& character, int itemIndex) {
    if (itemIndex < 0 || itemIndex >= character.inventory.size()) {
        lastActionMessage = "Invalid item selection.";
        return;
    }
    Item selectedItem = character.inventory[itemIndex];
    bool consumed = false;

    if (selectedItem.name == ITEM_HEALTH_POT) {
        character.health = std::min(character.health + 2, MAX_CHARACTER_HEALTH);
        lastActionMessage = character.name + " used " + selectedItem.name + ", restored health!";
        consumed = true;
    } else if (selectedItem.name == ITEM_ENERGY_DRINK) {
        character.hasEnergyDrink = true;
        lastActionMessage = character.name + " used " + selectedItem.name + ", gets another action!";
        consumed = true;
    } else if (selectedItem.name == ITEM_KNIFE || selectedItem.name == ITEM_GOGGLES) {
        lastActionMessage = selectedItem.name + " is a passive item.";
    } else {
        lastActionMessage = "Cannot use " + selectedItem.name + " this way.";
    }
    if (consumed) {
        character.inventory.erase(character.inventory.begin() + itemIndex);
        UpdatePassiveItemFlags(character); 
    }
}
void DrawCharacterUI(const Character& c, int x, int y, bool isCurrentPlayer) {
    DrawText(TextFormat("%s HP: %d/%d", c.name.c_str(), c.health, MAX_CHARACTER_HEALTH), x, y, 20, (c.health <= 1 ? RED : (isCurrentPlayer ? DARKGREEN : BLACK)));
    
    std::string invText = "Inventory (" + std::to_string(c.inventory.size()) + "/" + std::to_string(MAX_INVENTORY_SIZE) + "):";
    DrawText(invText.c_str(), x, y + 25, 18, BLACK);
    for (int i = 0; i < c.inventory.size(); i++) {
        DrawText(TextFormat("[%d] %s", i + 1, c.inventory[i].name.c_str()), x + 10, y + 50 + i * 20, 16, DARKGRAY);
    }
    int statusOffsetY = y + 50 + MAX_INVENTORY_SIZE * 20 + 5;
    if (c.hasKnife) {
        DrawText("Status: Knife Equipped", x, statusOffsetY, 16, ORANGE);
        statusOffsetY += 20;
    }
    if (c.hasGoggles) {
        std::string gogglesText = "Status: Goggles Equipped";
        if (isCurrentPlayer && bulletChamber != -1) {
            gogglesText += (currentChamber == bulletChamber) ? " (DANGER!)" : " (Chamber Clear)";
        }
        DrawText(gogglesText.c_str(), x, statusOffsetY, 16, c.hasGoggles && isCurrentPlayer && currentChamber == bulletChamber ? RED : BLUE);
        statusOffsetY += 20;
    }
     if (c.hasEnergyDrink && isCurrentPlayer) { 
        DrawText("Status: Energy Drink Active!", x, statusOffsetY, 16, PURPLE);
    }
}
void SpinRevolver() {
    bulletChamber = std::rand() % CYLINDER_SIZE;
    currentChamber = 0; 
}
void EndTurn(bool& currentTurnFlag, Character& currentCharacter) {
    if (currentCharacter.hasEnergyDrink) {
        currentCharacter.hasEnergyDrink = false; 
        lastActionMessage = currentCharacter.name + " gets another action!";
    } else {
        currentTurnFlag = !currentTurnFlag; 
        if (!gameOver) {
            SpinRevolver();
            lastActionMessage = "Cylinder spun. It's " + std::string(playerTurn ? player.name.c_str() : ai.name.c_str()) + "'s turn.";
        }
    }
}
void HandleShooting(Character& shooter, Character& target) {
    if (gameOver) return;

    bool isSelfTarget = (&shooter == &target);
    lastActionMessage = shooter.name + " shoots at " + (isSelfTarget ? "themselves" : target.name) + "... ";

    if (bulletChamber == -1) { // Should not happen with proper spinning
        lastActionMessage += "Error! No bullet in cylinder?";
        SpinRevolver(); // Safety spin
        EndTurn(playerTurn, shooter);
        return;
    }
    if (currentChamber == bulletChamber) {
        lastActionMessage += "BANG!!!";
        int damage = shooter.hasKnife ? 2 : 1;
        target.health -= damage;
        bulletChamber = -1; // Bullet is fired, cylinder effectively empty until next proper spin
    } else {
        lastActionMessage += "Click.";
    }
    currentChamber = (currentChamber + 1) % CYLINDER_SIZE;

    if (player.health <= 0 || ai.health <= 0) {
        gameOver = true;
        gameStatusMessage = (player.health <= 0) ? "AI Wins!" : "Player Wins!";
        lastActionMessage = "Game Over!";
    } else {
        EndTurn(playerTurn, shooter);
    }
}
void AiDecisionAndAction() {
    if (gameOver || playerTurn) return;
    if (ai.health <= 1) {
        for (int i = 0; i < ai.inventory.size(); ++i) {
            if (ai.inventory[i].name == ITEM_HEALTH_POT) {
                UseItemFromInventory(ai, i);
                if (!ai.hasEnergyDrink) EndTurn(playerTurn, ai);
                else ai.hasEnergyDrink = false;
                return;
            }
        }
    }
    if (!ai.hasEnergyDrink) {
        for (int i = 0; i < ai.inventory.size(); ++i) {
            if (ai.inventory[i].name == ITEM_ENERGY_DRINK) {
                 UseItemFromInventory(ai, i);
                 break; 
            }
        }
    }
    bool dangerSeen = ai.hasGoggles && (bulletChamber != -1 && currentChamber == bulletChamber);
    int choice = std::rand() % 100;

    if (dangerSeen) {
        if (choice < 60) {
            HandleShooting(ai, player);
        } else if (ai.inventory.size() < MAX_INVENTORY_SIZE) { 
            GiveItemToCharacter(ai, GetRandomGameItem());
            EndTurn(playerTurn, ai);
        } else {
             HandleShooting(ai, player);
        }
    } else { 
        if (choice < 35) {
            HandleShooting(ai, ai);
        } else if (choice < 70) { 
            HandleShooting(ai, player);
        } else if (ai.inventory.size() < MAX_INVENTORY_SIZE) { 
            GiveItemToCharacter(ai, GetRandomGameItem());
            EndTurn(playerTurn, ai);
        } else { 
            lastActionMessage = ai.name + " skips the turn.";
            currentChamber = (currentChamber + 1) % CYLINDER_SIZE;
            EndTurn(playerTurn, ai);
        }
    }
}

void ResetGame() {
    player = Character{}; // Re-initialize with default constructor
    ai = Character{};
    player.name = "Player"; // Set names if not in constructor
    ai.name = "AI";
    UpdatePassiveItemFlags(player); // Ensure flags are reset
    UpdatePassiveItemFlags(ai);

    gameOver = false;
    playerTurn = true;
    SpinRevolver();
    gameStatusMessage = "New Game! Player's turn.";
    lastActionMessage = "Cylinder spun.";
}


//------------------------------------------------------------------------------------
// Main entry point
//------------------------------------------------------------------------------------
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Russian Roulette Game");
    SetTargetFPS(60);
    std::srand(static_cast<unsigned int>(time(nullptr))); // Seed random number generator

    // Initialize player and AI names if not done by default constructor
    player.name = "Player";
    ai.name = "AI";
    ResetGame(); // Initial setup

    // Main game loop
    while (!WindowShouldClose()) {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------
        if (!gameOver) {
            if (playerTurn) {
                if (IsButtonClicked(shootSelfBtn)) {
                    HandleShooting(player, player);
                } else if (IsButtonClicked(shootEnemyBtn)) {
                    HandleShooting(player, ai);
                } else if (IsButtonClicked(getItemBtn)) {
                    GiveItemToCharacter(player, GetRandomGameItem());
                    EndTurn(playerTurn, player); // Getting an item is an action
                } else if (IsButtonClicked(skipTurnBtn)) {
                    lastActionMessage = player.name + " skips the turn.";
                    currentChamber = (currentChamber + 1) % CYLINDER_SIZE; // Advance chamber
                    EndTurn(playerTurn, player);
                }
                // Handle item usage from inventory buttons
                else if (IsButtonClicked(useItem1Btn) && player.inventory.size() > 0) {
                    UseItemFromInventory(player, 0);
                    // Using an item might grant an extra turn via Energy Drink, handled in UseItem & EndTurn
                } else if (IsButtonClicked(useItem2Btn) && player.inventory.size() > 1) {
                    UseItemFromInventory(player, 1);
                } else if (IsButtonClicked(useItem3Btn) && player.inventory.size() > 2) {
                    UseItemFromInventory(player, 2);
                }

            } else { // AI's turn
                // Simple delay for AI "thinking"
                static float aiThinkTimer = 0.0f;
                aiThinkTimer += GetFrameTime();
                if (aiThinkTimer >= 1.0f) { // AI acts after 1 second
                    AiDecisionAndAction();
                    aiThinkTimer = 0.0f;
                }
            }
        } else { // Game Over
            if (IsKeyPressed(KEY_ENTER)) {
                ResetGame();
            }
        }

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw character panels (from your initial main.cpp, adapted)
        DrawCharacterUI(player, 30, 30, playerTurn);
        DrawCharacterUI(ai, SCREEN_WIDTH - 280, 30, !playerTurn); // Adjusted X for AI

        // Draw game status messages
        DrawText(gameStatusMessage.c_str(), SCREEN_WIDTH/2 - MeasureText(gameStatusMessage.c_str(), 20)/2, 10, 20, BLACK);
        DrawText(lastActionMessage.c_str(), 10, SCREEN_HEIGHT - 30, 18, DARKBLUE);

        if (!gameOver) {
            // Draw buttons (from your initial main.cpp, text centered)
            Color btnEnabledColor = playerTurn ? SKYBLUE : LIGHTGRAY;
            Color btnDisabledColor = LIGHTGRAY;

            DrawRectangleRec(shootSelfBtn, playerTurn ? btnEnabledColor : btnDisabledColor);
            DrawText("Shoot Self", shootSelfBtn.x + (shootSelfBtn.width - MeasureText("Shoot Self", 20))/2, shootSelfBtn.y + 10, 20, BLACK);

            DrawRectangleRec(shootEnemyBtn, playerTurn ? btnEnabledColor : btnDisabledColor);
            DrawText("Shoot Enemy", shootEnemyBtn.x + (shootEnemyBtn.width - MeasureText("Shoot Enemy", 20))/2, shootEnemyBtn.y + 10, 20, BLACK);
            
            DrawRectangleRec(getItemBtn, playerTurn ? btnEnabledColor : btnDisabledColor);
            DrawText("Get Item", getItemBtn.x + (getItemBtn.width - MeasureText("Get Item", 20))/2, getItemBtn.y + 10, 20, BLACK);

            DrawRectangleRec(skipTurnBtn, playerTurn ? btnEnabledColor : btnDisabledColor);
            DrawText("Skip Turn", skipTurnBtn.x + (skipTurnBtn.width - MeasureText("Skip Turn", 20))/2, skipTurnBtn.y + 10, 20, BLACK);

            // Draw "Use Item" buttons
            if (playerTurn) {
                if (player.inventory.size() > 0) {
                    DrawRectangleRec(useItem1Btn, SKYBLUE);
                    DrawText(TextFormat("Use: %s", player.inventory[0].name.substr(0,15).c_str()), useItem1Btn.x + 10, useItem1Btn.y + 5, 18, BLACK);
                }
                if (player.inventory.size() > 1) {
                    DrawRectangleRec(useItem2Btn, SKYBLUE);
                    DrawText(TextFormat("Use: %s", player.inventory[1].name.substr(0,15).c_str()), useItem2Btn.x + 10, useItem2Btn.y + 5, 18, BLACK);
                }
                if (player.inventory.size() > 2) {
                    DrawRectangleRec(useItem3Btn, SKYBLUE);
                    DrawText(TextFormat("Use: %s", player.inventory[2].name.substr(0,15).c_str()), useItem3Btn.x + 10, useItem3Btn.y + 5, 18, BLACK);
                }
            }


        } else { // Game Over Screen
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 40) / 2, SCREEN_HEIGHT / 2 - 40, 40, RED);
            // gameStatusMessage already contains winner info
            DrawText("Press [ENTER] to Play Again", SCREEN_WIDTH / 2 - MeasureText("Press [ENTER] to Play Again", 20) / 2, SCREEN_HEIGHT / 2 + 50, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
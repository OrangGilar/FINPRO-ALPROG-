#include "ui.h"
#include "globals.h"
#include "constants.h"
#include "raylib.h"
#include <string> 

void DrawCharacterStatusUI(const Character& c, int x, int y, bool isTurn) {
    DrawText(TextFormat("%s HP: %d/%d", c.name.c_str(), c.health, MAX_GAME_HEALTH), x, y, 20, isTurn ? DARKGREEN : BLACK);
    
    int statusOffsetY = y + 25;
    if (c.hasKnife) {
        DrawText("KNIFE EQUIPPED (Dmg x2)", x, statusOffsetY, 18, ORANGE);
        statusOffsetY += 20;
    }
    if (c.hasGoggles && isTurn) {
        std::string gogglesMsg = "GOGGLES: Chamber ";
        gogglesMsg += (currentChamber == bulletChamber) ? "LIVE (Danger!)" : "CLEAR.";
        DrawText(gogglesMsg.c_str(), x, statusOffsetY, 18, (currentChamber == bulletChamber) ? RED : BLUE);
        statusOffsetY += 20;
    }
    if (c.hasEnergyDrink && isTurn) {
        DrawText("ENERGY DRINK ACTIVE!", x, statusOffsetY, 18, PURPLE);
        statusOffsetY += 20;
    }

    std::string invDisplay = "Inventory [" + std::to_string(c.inventory.size()) + "/" + std::to_string(MAX_INVENTORY_SIZE) + "]:";
    DrawText(invDisplay.c_str(), x, statusOffsetY, 18, DARKGRAY);
    statusOffsetY += 25;
    for (int i = 0; i < c.inventory.size(); i++) {
        DrawText(TextFormat("[%d] %s", i + 1, c.inventory[i].name.c_str()), x + 10, statusOffsetY + i * 20, 16, GRAY);
    }
}

void DrawGameUI() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    Character& currentPlayer = player1Turn ? player1 : player2; 

    DrawCharacterStatusUI(player1, 30, 30, player1Turn && !gameOver);
    DrawCharacterStatusUI(player2, SCREEN_WIDTH - 300, 30, !player1Turn && !gameOver);

    DrawText(message.c_str(), 30, SCREEN_HEIGHT - 30, 20, DARKBLUE);

    if (!gameOver) {
        if (showItemChoice) {
            if (choosingSwapOrDiscard) { 
                std::string invFullMsg = currentPlayer.name + " found: " + currentFoundItem.name + ". Inventory FULL!";
                 DrawText(invFullMsg.c_str(),
                     SCREEN_WIDTH / 2 - MeasureText(invFullMsg.c_str(), 20) / 2,
                     SCREEN_HEIGHT - 280, 20, RED);
                DrawText("Swap with an item in inventory or Discard the new item?",
                     SCREEN_WIDTH / 2 - MeasureText("Swap with an item in inventory or Discard the new item?", 20) / 2,
                     SCREEN_HEIGHT - 250, 20, DARKBLUE);

                DrawRectangleRec(btnDiscardNewFoundItem, ORANGE);
                DrawText("Discard New Item", btnDiscardNewFoundItem.x + (btnDiscardNewFoundItem.width - MeasureText("Discard New Item", 20))/2, btnDiscardNewFoundItem.y + 10, 20, BLACK);

                if (currentPlayer.inventory.size() >= 1) {
                    DrawRectangleRec(btnReplaceSlot1, LIME);
                    DrawText("Swap Slot 1", btnReplaceSlot1.x + (btnReplaceSlot1.width - MeasureText("Swap Slot 1", 18))/2, btnReplaceSlot1.y + 11, 18, BLACK);
                }
                if (currentPlayer.inventory.size() >= 2) {
                    DrawRectangleRec(btnReplaceSlot2, LIME);
                    DrawText("Swap Slot 2", btnReplaceSlot2.x + (btnReplaceSlot2.width - MeasureText("Swap Slot 2", 18))/2, btnReplaceSlot2.y + 11, 18, BLACK);
                }
                if (currentPlayer.inventory.size() >= 3) {
                    DrawRectangleRec(btnReplaceSlot3, LIME);
                    DrawText("Swap Slot 3", btnReplaceSlot3.x + (btnReplaceSlot3.width - MeasureText("Swap Slot 3", 18))/2, btnReplaceSlot3.y + 11, 18, BLACK);
                }
            } else { 
                std::string foundItemMsgText = currentPlayer.name + " found: " + currentFoundItem.name;
                DrawText(foundItemMsgText.c_str(),
                         SCREEN_WIDTH / 2 - MeasureText(foundItemMsgText.c_str(), 20) / 2,
                         SCREEN_HEIGHT - 280, 20, DARKBLUE);

                DrawRectangleRec(btnUseFoundItem, ORANGE); 
                DrawText("Use Now", btnUseFoundItem.x + (btnUseFoundItem.width - MeasureText("Use Now", 20))/2, btnUseFoundItem.y + 10, 20, BLACK);

                DrawRectangleRec(btnStoreFoundItem, ORANGE);
                DrawText("Store Item", btnStoreFoundItem.x + (btnStoreFoundItem.width - MeasureText("Store Item", 20))/2, btnStoreFoundItem.y + 10, 20, BLACK);
            }
        } else if (choosingItemToUseFromInventory) { 
            DrawText(TextFormat("%s: Choose an item to use", currentPlayer.name.c_str()),
                     SCREEN_WIDTH/2 - MeasureText(TextFormat("%s: Choose an item to use", currentPlayer.name.c_str()), 20)/2,
                     SCREEN_HEIGHT - 240, 20, DARKBLUE);
            
            if (currentPlayer.inventory.size() >= 1) {
                DrawRectangleRec(btnChooseInvItem1, SKYBLUE);
                DrawText(TextFormat("Use: %s", currentPlayer.inventory[0].name.c_str()), btnChooseInvItem1.x + 20, btnChooseInvItem1.y + 10, 20, BLACK);
            }
            if (currentPlayer.inventory.size() >= 2) {
                DrawRectangleRec(btnChooseInvItem2, SKYBLUE);
                DrawText(TextFormat("Use: %s", currentPlayer.inventory[1].name.c_str()), btnChooseInvItem2.x + 20, btnChooseInvItem2.y + 10, 20, BLACK);
            }
            if (currentPlayer.inventory.size() >= 3) {
                DrawRectangleRec(btnChooseInvItem3, SKYBLUE);
                DrawText(TextFormat("Use: %s", currentPlayer.inventory[2].name.c_str()), btnChooseInvItem3.x + 20, btnChooseInvItem3.y + 10, 20, BLACK);
            }
            DrawRectangleRec(btnCancelUseStoredItem, LIGHTGRAY);
            DrawText("Cancel", btnCancelUseStoredItem.x + (btnCancelUseStoredItem.width - MeasureText("Cancel", 20))/2, btnCancelUseStoredItem.y + 5, 20, BLACK);

        } else { 
            DrawRectangleRec(btnShootSelf, SKYBLUE);
            DrawText("Shoot Self", btnShootSelf.x + (btnShootSelf.width - MeasureText("Shoot Self", 20))/2, btnShootSelf.y + 10, 20, BLACK);

            DrawRectangleRec(btnShootOther, SKYBLUE);
            DrawText("Shoot Opponent", btnShootOther.x + (btnShootOther.width - MeasureText("Shoot Opponent", 20))/2, btnShootOther.y + 10, 20, BLACK);

            DrawRectangleRec(btnSkipTurn, SKYBLUE);
            DrawText("Skip Turn", btnSkipTurn.x + (btnSkipTurn.width - MeasureText("Skip Turn", 20))/2, btnSkipTurn.y + 10, 20, BLACK);
        
            DrawRectangleRec(btnSortInventory, LIGHTGRAY);
            DrawText("Sort Inventory", btnSortInventory.x + (btnSortInventory.width - MeasureText("Sort Inventory", 20))/2, btnSortInventory.y + 10, 20, BLACK);
        
            DrawRectangleRec(btnUseStoredItem, LIME); 
            DrawText("Use Stored Item", btnUseStoredItem.x + (btnUseStoredItem.width - MeasureText("Use Stored Item", 20))/2, btnUseStoredItem.y + 10, 20, BLACK);
        }
    } else { 
        std::string winnerMsgText = (player1.health <= 0 ? player2.name + " Wins!" : player1.name + " Wins!");
        DrawText(winnerMsgText.c_str(), SCREEN_WIDTH/2 - MeasureText(winnerMsgText.c_str(), 40)/2, SCREEN_HEIGHT/2 - 40, 40, RED);
        DrawText("CLICK to Play Again", SCREEN_WIDTH/2 - MeasureText("CLICK to Play Again", 20)/2, SCREEN_HEIGHT/2 + 20, 20, DARKGRAY);
    }

    EndDrawing();
}
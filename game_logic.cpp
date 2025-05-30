#include "game_logic.h"
#include "globals.h"    
#include "constants.h"  
#include "binary_logging.h" 
#include "raylib.h"     
#include <algorithm>   
#include <string>      
#include <vector>      
#include <ctime>       

void SpinCylinder() {
    bulletChamber = std::rand() % CYLINDER_SIZE;
    currentChamber = 0;
}

void AdvanceChamber() {
    currentChamber = (currentChamber + 1) % CYLINDER_SIZE;
}

bool IsButtonClicked(Rectangle btn) {
    return CheckCollisionPointRec(GetMousePosition(), btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

Item GetRandomItem() {
    if (allItems.empty()) { 
        return {"Error Item", "No items defined", "Error", 0};
    }
    return allItems[std::rand() % allItems.size()];
}

void MaybeGetItemForCurrentPlayer() {
    Character& currentPlayer = player1Turn ? player1 : player2;
    showItemChoice = false; 
    choosingSwapOrDiscard = false;
    choosingItemToUseFromInventory = false; 

    if ((std::rand() % 100) < ITEM_FIND_CHANCE_PERCENT) { 
        currentFoundItem = GetRandomItem();
        showItemChoice = true; 

        if (currentPlayer.inventory.size() >= MAX_INVENTORY_SIZE) {
            choosingSwapOrDiscard = true; 
            message = currentPlayer.name + " found " + currentFoundItem.name + " but inventory is FULL! Swap or Discard new item?";
        } else {
            message = currentPlayer.name + " found: " + currentFoundItem.name + "! Use or Store?";
        }
    }
}

void AddItemToInventory(Character &c, const Item& item) {
    if (c.inventory.size() < MAX_INVENTORY_SIZE) {
        c.inventory.push_back(item);
        message = c.name + " stored " + item.name + ".";
        CreateAndLogEvent(GameEventType::ITEM_FOUND_STORED, c.name, item.name, "Added to inventory");
    } else {
        message = c.name + "'s inventory is full! Cannot store " + item.name + ".";
    }
}

void ActivateItemEffect(Character &c, const Item& item) {
    message = c.name + " used " + item.name + ".";
    std::string effectDetails = "Effect applied";

    if (item.name == "Health Potion") {
        if (c.health < MAX_GAME_HEALTH) {
            c.health++;
            message += " Healed 1 HP!";
            effectDetails = "Health restored to " + std::to_string(c.health);
        } else {
            message += " Already at max health!";
            effectDetails = "Already max health";
        }
    } else if (item.name == "Knife") {
        c.hasKnife = true;
        message += " Knife equipped! Damage increased.";
        effectDetails = "Knife active";
    } else if (item.name == "Goggles") {
        c.hasGoggles = true;
        message += " Goggles equipped! Chamber status visible.";
        effectDetails = "Goggles active";
    } else if (item.name == "Energy Drink") {
        c.hasEnergyDrink = true;
        message += " Gets an extra action this turn!";
        effectDetails = "Energy Drink active";
    }
    CreateAndLogEvent(GameEventType::ITEM_USED, c.name, item.name, effectDetails);
}

void RemoveItemFromInventoryByName(Character& c, const std::string& itemName) {
    auto it = std::find_if(c.inventory.begin(), c.inventory.end(),
        [&](const Item& item_in_inventory) { return item_in_inventory.name == itemName; });

    if (it != c.inventory.end()) {
        c.inventory.erase(it);
    }
}

void SortCharacterInventory(Character& c) {
    if (c.inventory.empty()) {
        message = c.name + "'s inventory is empty, nothing to sort.";
        return;
    }
    std::stable_sort(c.inventory.begin(), c.inventory.end(), [](const Item& a, const Item& b) {
        return a.name < b.name; 
    });
    message = c.name + " sorted their inventory.";
}

void EndPlayerTurn(Character& actingPlayer, bool actionTookPlace) {
    if (actionTookPlace && actingPlayer.hasEnergyDrink) {
        message = actingPlayer.name + " used Energy Drink for an extra action!";
        CreateAndLogEvent(GameEventType::ITEM_USED, actingPlayer.name, "Energy Drink", "Took extra turn effect");
        actingPlayer.hasEnergyDrink = false;
        showItemChoice = false; 
        choosingSwapOrDiscard = false;
        choosingItemToUseFromInventory = false; 
    } else {
        player1Turn = !player1Turn;
        showItemChoice = false;
        choosingSwapOrDiscard = false;
        choosingItemToUseFromInventory = false; 
        if (!gameOver) {
             Character& nextPlayer = player1Turn ? player1 : player2;
             MaybeGetItemForCurrentPlayer(); 
             if (!showItemChoice) { 
                message = "It's " + nextPlayer.name + "'s turn.";
             }
        }
    }
}

void HandleShooting(Character& shooter, Character& target) {
    if (gameOver) return;

    bool isSelf = (&shooter == &target);
    std::string initialMessage = shooter.name + (isSelf ? " shoots themselves." : " shoots " + target.name + ".");
    
    SpinCylinder(); 
    initialMessage += " Cylinder spun...";

    std::string outcomeDetail;
    GameEventType shotEventType = isSelf ? GameEventType::SHOT_SELF : GameEventType::SHOT_OPPONENT;

    if (currentChamber == bulletChamber) {
        initialMessage += " BANG!!!";
        int damage = shooter.hasKnife ? 2 : 1;
        int healthBefore = target.health;
        target.health -= damage;
        outcomeDetail = "Dealt " + std::to_string(damage) + " dmg. HP: " + std::to_string(healthBefore) + " -> " + std::to_string(target.health);
        initialMessage += " " + target.name + " takes " + std::to_string(damage) + " damage.";
        if(shooter.hasKnife) {
            initialMessage += " (Knife hit!)";
            outcomeDetail += " (Knife)";
        }
    } else {
        initialMessage += " Click...";
        outcomeDetail = "Click. No damage.";
    }
    message = initialMessage; 
    CreateAndLogEvent(shotEventType, shooter.name, target.name, outcomeDetail); // Target name in itemName field for context

    currentChamber = (currentChamber + 1) % CYLINDER_SIZE;

    if (player1.health <= 0 || player2.health <= 0) {
        gameOver = true;
        std::string winnerName = (player1.health <= 0 ? player2.name : player1.name);
        std::string loserName = (player1.health <= 0 ? player1.name : player2.name);
        CreateAndLogEvent(GameEventType::GAME_OVER, winnerName, "", "Defeated " + loserName);
    } else {
        EndPlayerTurn(shooter, true);
    }
}

void ResetGame() {
    player1.name = "Player 1";
    player1.health = MAX_GAME_HEALTH;
    player1.inventory.clear();
    player1.hasKnife = false;
    player1.hasGoggles = false;
    player1.hasEnergyDrink = false;

    player2.name = "Player 2";
    player2.health = MAX_GAME_HEALTH;
    player2.inventory.clear();
    player2.hasKnife = false;
    player2.hasGoggles = false;
    player2.hasEnergyDrink = false;

    player1Turn = true;
    gameOver = false;
    SpinCylinder(); 
    
    showItemChoice = false;
    choosingSwapOrDiscard = false;
    choosingItemToUseFromInventory = false; 

    Character& firstPlayer = player1Turn ? player1 : player2;
    MaybeGetItemForCurrentPlayer(); 
    if (!showItemChoice) {
         message = "Game (re)started! " + firstPlayer.name + "'s turn.";
    }
    CreateAndLogEvent(GameEventType::GAME_START, "System", "", "New game started");
}

void UpdateGame() {
    Character& currentPlayer = player1Turn ? player1 : player2;
    Character& otherPlayer = player1Turn ? player2 : player1;

    if (!gameOver) {
        if (showItemChoice) {
            if (choosingSwapOrDiscard) { 
                if (IsButtonClicked(btnDiscardNewFoundItem)) {
                    message = currentPlayer.name + " discarded " + currentFoundItem.name + ".";
                    CreateAndLogEvent(GameEventType::ITEM_FOUND_DISCARDED, currentPlayer.name, currentFoundItem.name, "Inventory full");
                    showItemChoice = false;
                    choosingSwapOrDiscard = false;
                    EndPlayerTurn(currentPlayer, false); 
                } else if (IsButtonClicked(btnReplaceSlot1) && currentPlayer.inventory.size() >= 1) {
                    std::string replacedItemName = currentPlayer.inventory[0].name;
                    message = currentPlayer.name + " swapped " + replacedItemName + " for " + currentFoundItem.name + ".";
                    currentPlayer.inventory[0] = currentFoundItem;
                    CreateAndLogEvent(GameEventType::ITEM_FOUND_SWAPPED, currentPlayer.name, currentFoundItem.name, "Replaced " + replacedItemName);
                    showItemChoice = false;
                    choosingSwapOrDiscard = false;
                    EndPlayerTurn(currentPlayer, false);
                } else if (IsButtonClicked(btnReplaceSlot2) && currentPlayer.inventory.size() >= 2) {
                    std::string replacedItemName = currentPlayer.inventory[1].name;
                    message = currentPlayer.name + " swapped " + replacedItemName + " for " + currentFoundItem.name + ".";
                    currentPlayer.inventory[1] = currentFoundItem;
                     CreateAndLogEvent(GameEventType::ITEM_FOUND_SWAPPED, currentPlayer.name, currentFoundItem.name, "Replaced " + replacedItemName);
                    showItemChoice = false;
                    choosingSwapOrDiscard = false;
                    EndPlayerTurn(currentPlayer, false);
                } else if (IsButtonClicked(btnReplaceSlot3) && currentPlayer.inventory.size() >= 3) {
                    std::string replacedItemName = currentPlayer.inventory[2].name;
                    message = currentPlayer.name + " swapped " + replacedItemName + " for " + currentFoundItem.name + ".";
                    currentPlayer.inventory[2] = currentFoundItem;
                    CreateAndLogEvent(GameEventType::ITEM_FOUND_SWAPPED, currentPlayer.name, currentFoundItem.name, "Replaced " + replacedItemName);
                    showItemChoice = false;
                    choosingSwapOrDiscard = false;
                    EndPlayerTurn(currentPlayer, false);
                }
            } else { 
                if (IsButtonClicked(btnUseFoundItem)) {
                    ActivateItemEffect(currentPlayer, currentFoundItem); 
                    CreateAndLogEvent(GameEventType::ITEM_FOUND_USED_NOW, currentPlayer.name, currentFoundItem.name, "Used immediately after finding");
                    showItemChoice = false;
                    EndPlayerTurn(currentPlayer, true); 
                } else if (IsButtonClicked(btnStoreFoundItem)) {
                    AddItemToInventory(currentPlayer, currentFoundItem); 
                    showItemChoice = false;
                    EndPlayerTurn(currentPlayer, false); 
                }
            }
        } else if (choosingItemToUseFromInventory) { 
            if (IsButtonClicked(btnChooseInvItem1) && currentPlayer.inventory.size() >= 1) {
                Item itemToUse = currentPlayer.inventory[0];
                ActivateItemEffect(currentPlayer, itemToUse); 
                RemoveItemFromInventoryByName(currentPlayer, itemToUse.name);
                choosingItemToUseFromInventory = false;
                EndPlayerTurn(currentPlayer, true);
            } else if (IsButtonClicked(btnChooseInvItem2) && currentPlayer.inventory.size() >= 2) {
                Item itemToUse = currentPlayer.inventory[1];
                ActivateItemEffect(currentPlayer, itemToUse); 
                RemoveItemFromInventoryByName(currentPlayer, itemToUse.name);
                choosingItemToUseFromInventory = false;
                EndPlayerTurn(currentPlayer, true);
            } else if (IsButtonClicked(btnChooseInvItem3) && currentPlayer.inventory.size() >= 3) {
                Item itemToUse = currentPlayer.inventory[2];
                ActivateItemEffect(currentPlayer, itemToUse); 
                RemoveItemFromInventoryByName(currentPlayer, itemToUse.name);
                choosingItemToUseFromInventory = false;
                EndPlayerTurn(currentPlayer, true);
            } else if (IsButtonClicked(btnCancelUseStoredItem)) {
                choosingItemToUseFromInventory = false;
                message = "Item use cancelled. " + currentPlayer.name + "'s turn.";
            }
        } else { 
            if (IsButtonClicked(btnShootSelf)) {
                HandleShooting(currentPlayer, currentPlayer);
            } else if (IsButtonClicked(btnShootOther)) {
                HandleShooting(currentPlayer, otherPlayer);
            } else if (IsButtonClicked(btnSkipTurn)) {
                message = currentPlayer.name + " skipped their turn.";
                currentChamber = (currentChamber + 1) % CYLINDER_SIZE;
                CreateAndLogEvent(GameEventType::TURN_SKIPPED, currentPlayer.name, "", "Chamber advanced");
                EndPlayerTurn(currentPlayer, true);
            } else if (IsButtonClicked(btnSortInventory)) { 
                SortCharacterInventory(currentPlayer);
            } else if (IsButtonClicked(btnUseStoredItem)) { 
                if (!currentPlayer.inventory.empty()) {
                    choosingItemToUseFromInventory = true; 
                    message = currentPlayer.name + ", select an item to use:";
                } else {
                    message = currentPlayer.name + "'s inventory is empty!";
                }
            }
        }
    } else { 
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ResetGame();
        }
    }
}
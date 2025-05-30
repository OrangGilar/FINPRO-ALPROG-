#include "logic.h"
#include "globals.h"
#include "constants.h"
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
        return {"Error Item", "No items defined"};
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
    } else {
        message = c.name + "'s inventory is full! Cannot store " + item.name + ".";
    }
}

void ActivateItemEffect(Character &c, const Item& item) {
    message = c.name + " used " + item.name + ".";

    if (item.name == "Health Potion") {
        if (c.health < MAX_GAME_HEALTH) {
            c.health++;
            message += " Healed 1 HP!";
        } else {
            message += " Already at max health!";
        }
    } else if (item.name == "Knife") {
        c.hasKnife = true;
        message += " Knife equipped! Damage increased.";
    } else if (item.name == "Goggles") {
        c.hasGoggles = true;
        message += " Goggles equipped! Chamber status visible.";
    } else if (item.name == "Energy Drink") {
        c.hasEnergyDrink = true;
        message += " Gets an extra action this turn!";
    }
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
    message = shooter.name + (isSelf ? " shoots themselves." : " shoots " + target.name + ".");
    
    SpinCylinder(); 
    message += " Cylinder spun...";

    if (currentChamber == bulletChamber) {
        message += " BANG!!!";
        int damage = shooter.hasKnife ? 2 : 1;
        target.health -= damage;
        message += " " + target.name + " takes " + std::to_string(damage) + " damage.";
        if(shooter.hasKnife) message += " (Knife hit!)";
        
    } else {
        message += " Click...";
    }
    currentChamber = (currentChamber + 1) % CYLINDER_SIZE;

    if (player1.health <= 0 || player2.health <= 0) {
        gameOver = true;
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
}

void UpdateGame() {
    Character& currentPlayer = player1Turn ? player1 : player2;
    Character& otherPlayer = player1Turn ? player2 : player1;

    if (!gameOver) {
        if (showItemChoice) {
            if (choosingSwapOrDiscard) { 
                if (IsButtonClicked(btnDiscardNewFoundItem)) {
                    message = currentPlayer.name + " discarded " + currentFoundItem.name + ".";
                    showItemChoice = false;
                    choosingSwapOrDiscard = false;
                    EndPlayerTurn(currentPlayer, false); 
                } else if (IsButtonClicked(btnReplaceSlot1) && currentPlayer.inventory.size() >= 1) {
                    message = currentPlayer.name + " swapped " + currentPlayer.inventory[0].name + " for " + currentFoundItem.name + ".";
                    currentPlayer.inventory[0] = currentFoundItem;
                    showItemChoice = false;
                    choosingSwapOrDiscard = false;
                    EndPlayerTurn(currentPlayer, false);
                } else if (IsButtonClicked(btnReplaceSlot2) && currentPlayer.inventory.size() >= 2) {
                    message = currentPlayer.name + " swapped " + currentPlayer.inventory[1].name + " for " + currentFoundItem.name + ".";
                    currentPlayer.inventory[1] = currentFoundItem;
                    showItemChoice = false;
                    choosingSwapOrDiscard = false;
                    EndPlayerTurn(currentPlayer, false);
                } else if (IsButtonClicked(btnReplaceSlot3) && currentPlayer.inventory.size() >= 3) {
                    message = currentPlayer.name + " swapped " + currentPlayer.inventory[2].name + " for " + currentFoundItem.name + ".";
                    currentPlayer.inventory[2] = currentFoundItem;
                    showItemChoice = false;
                    choosingSwapOrDiscard = false;
                    EndPlayerTurn(currentPlayer, false);
                }
            } else { 
                if (IsButtonClicked(btnUseFoundItem)) {
                    ActivateItemEffect(currentPlayer, currentFoundItem); 
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
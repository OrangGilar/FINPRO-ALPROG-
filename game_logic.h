#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "raylib.h"
#include "items.h"   
#include "characters.h"
#include <string> 
#include <vector> 

void SpinCylinder();
void AdvanceChamber();
Item GetRandomItem(); 
void MaybeGetItemForCurrentPlayer();
void AddItemToInventory(Character &c, const Item& item);
void ActivateItemEffect(Character &c, const Item& item);
void RemoveItemFromInventoryByName(Character& c, const std::string& itemName);
void SortCharacterInventory(Character& c);
void EndPlayerTurn(Character& actingPlayer, bool actionTookPlace);
void HandleShooting(Character& shooter, Character& target);
void ResetGame();
bool IsButtonClicked(Rectangle btn);

void UpdateGame(); 

#endif // GAME_LOGIC_H
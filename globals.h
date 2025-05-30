#ifndef GLOBALS_H
#define GLOBALS_H

#include "raylib.h"
#include "items.h"
#include "characters.h"
#include <string>
#include <vector>

extern Character player1;
extern Character player2;
extern bool player1Turn;
extern bool gameOver;
extern int bulletChamber;
extern int currentChamber;

extern std::string message;
extern Item currentFoundItem;
extern bool showItemChoice;
extern bool choosingSwapOrDiscard;
extern bool choosingItemToUseFromInventory;

extern std::vector<Item> allItems;

extern Rectangle btnShootSelf;
extern Rectangle btnShootOther;
extern Rectangle btnSkipTurn;
extern Rectangle btnUseFoundItem;
extern Rectangle btnStoreFoundItem;
extern Rectangle btnSortInventory;
extern Rectangle btnUseStoredItem;
extern Rectangle btnDiscardNewFoundItem;
extern Rectangle btnReplaceSlot1;
extern Rectangle btnReplaceSlot2;
extern Rectangle btnReplaceSlot3;
extern Rectangle btnChooseInvItem1;
extern Rectangle btnChooseInvItem2;
extern Rectangle btnChooseInvItem3;
extern Rectangle btnCancelUseStoredItem;

#endif // GLOBALS_H
#include "globals.h"
#include "constants.h"

Character player1;
Character player2;
bool player1Turn = true;
bool gameOver = false;
int bulletChamber;
int currentChamber;

std::string message = "Welcome! Find items to gain an edge.";
Item currentFoundItem;
bool showItemChoice = false;
bool choosingSwapOrDiscard = false;
bool choosingItemToUseFromInventory = false;

std::vector<Item> allItems = {
    {"Health Potion", "Restores 1 HP"},
    {"Knife", "Deal 2 damage instead of 1"},
    {"Goggles", "See if current chamber is live"},
    {"Energy Drink", "Take an extra action this turn"}
};

Rectangle btnShootSelf = {50, SCREEN_HEIGHT - 240, 200, 40};
Rectangle btnShootOther = {300, SCREEN_HEIGHT - 240, 200, 40};
Rectangle btnSkipTurn = {550, SCREEN_HEIGHT - 240, 200, 40};

Rectangle btnUseFoundItem = { (float)SCREEN_WIDTH / 2 - 320, SCREEN_HEIGHT - 180, 200, 40 };
Rectangle btnStoreFoundItem = { (float)SCREEN_WIDTH / 2 - 100 , SCREEN_HEIGHT - 180, 200, 40 };
Rectangle btnSortInventory = { (float)SCREEN_WIDTH / 2 + 120, SCREEN_HEIGHT - 180, 200, 40 };

Rectangle btnUseStoredItem = { (float)SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 120, 200, 40 };

Rectangle btnDiscardNewFoundItem = { (float)SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT - 200, 220, 40 };
Rectangle btnReplaceSlot1 = { (float)SCREEN_WIDTH / 2 - 300 - 10, SCREEN_HEIGHT - 140, 190, 40 };
Rectangle btnReplaceSlot2 = { (float)SCREEN_WIDTH / 2 - 95, SCREEN_HEIGHT - 140, 190, 40 };
Rectangle btnReplaceSlot3 = { (float)SCREEN_WIDTH / 2 + 110, SCREEN_HEIGHT - 140, 190, 40 };

Rectangle btnChooseInvItem1 = { (float)SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 200, 300, 40 };
Rectangle btnChooseInvItem2 = { (float)SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 150, 300, 40 };
Rectangle btnChooseInvItem3 = { (float)SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 100, 300, 40 };
Rectangle btnCancelUseStoredItem = { (float)SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT - 50, 200, 30 };
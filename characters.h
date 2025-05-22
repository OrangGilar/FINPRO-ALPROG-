#ifndef CHARACTERS_H
#define CHARACTERS_H

#include <string>
#include <vector>
#include "items.h"

struct Character {
    std::string name;
    int health = 3;
    bool hasKnife = false;
    bool hasGoggles = false;
    bool hasEnergyDrink = false;
    std::vector<Item> inventory;
};

#endif // CHARACTERS_H
#ifndef ITEMS_H
#define ITEMS_H

#include <string>

struct Item {
    std::string name;
    std::string effect;
    std::string classification; 
    int probability_weight;   
};

#endif // ITEMS_H
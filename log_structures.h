#ifndef LOG_STRUCTURES_H
#define LOG_STRUCTURES_H

#include <ctime> 

const int LOG_NAME_FIELD_SIZE = 50; 
const int LOG_DETAILS_FIELD_SIZE = 100;

enum class GameEventType : char { 
    GAME_START,
    SHOT_SELF,
    SHOT_OPPONENT,
    ITEM_USED,
    ITEM_FOUND_STORED,
    ITEM_FOUND_USED_NOW, 
    ITEM_FOUND_DISCARDED,
    ITEM_FOUND_SWAPPED,
    TURN_SKIPPED,
    GAME_OVER 
};

struct GameEventLog {
    time_t timestamp;        
    GameEventType eventType;
    char playerName[LOG_NAME_FIELD_SIZE];
    char itemName[LOG_NAME_FIELD_SIZE];   
    char details[LOG_DETAILS_FIELD_SIZE]; 
    int player1Health; 
    int player2Health;
};

#endif // LOG_STRUCTURES_H
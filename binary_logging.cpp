#include "binary_logging.h"
#include "globals.h" 
#include <fstream>
#include <cstring> 
#include <iostream> 

void LogGameEventToFile(const GameEventLog& logEntry) {
    std::ofstream logFile("game_log.bin", std::ios::binary | std::ios::app);
    if (logFile.is_open()) {
        logFile.write(reinterpret_cast<const char*>(&logEntry), sizeof(GameEventLog));
        logFile.close();
    } else {
        std::cerr << "Error: Unable to open game_log.bin for writing." << std::endl;
    }
}

void CreateAndLogEvent(GameEventType type, 
                       const std::string& pName, 
                       const std::string& iName, 
                       const std::string& dtls) {
    GameEventLog entry = {}; 

    entry.timestamp = std::time(nullptr);
    entry.eventType = type;
    
    if (!pName.empty()) {
        strncpy(entry.playerName, pName.c_str(), LOG_NAME_FIELD_SIZE - 1);
        entry.playerName[LOG_NAME_FIELD_SIZE - 1] = '\0'; 
    }

    if (!iName.empty()) {
        strncpy(entry.itemName, iName.c_str(), LOG_NAME_FIELD_SIZE - 1);
        entry.itemName[LOG_NAME_FIELD_SIZE - 1] = '\0';
    }
    if (!dtls.empty()) {
        strncpy(entry.details, dtls.c_str(), LOG_DETAILS_FIELD_SIZE - 1);
        entry.details[LOG_DETAILS_FIELD_SIZE - 1] = '\0';
    }

    entry.player1Health = player1.health;
    entry.player2Health = player2.health;

    LogGameEventToFile(entry);
}
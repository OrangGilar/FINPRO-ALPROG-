#ifndef BINARY_LOGGING_H
#define BINARY_LOGGING_H

#include "log_structures.h" 
#include <string>
#include <vector> 

void LogGameEventToFile(const GameEventLog& logEntry);

void CreateAndLogEvent(GameEventType type, 
                       const std::string& playerName, 
                       const std::string& itemName = "", 
                       const std::string& details = "");

// std::vector<GameEventLog> ReadAllGameLogs(); // Optional for later

#endif // BINARY_LOGGING_H
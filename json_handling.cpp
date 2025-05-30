#include "json_handling.h"
#include "globals.h"      
#include "items.h"        
#include "json.hpp"       // nlohmann/json library
#include <fstream>       
#include <iostream>      

using json = nlohmann::json;

bool LoadItemsFromConfig(const std::string& configFilePath) {
    allItems.clear(); 
    std::ifstream configFile(configFilePath);

    if (!configFile.is_open()) {
        message = "Error: Could not open items config file: " + configFilePath;
        std::cerr << message << std::endl; 
        allItems.push_back({"Fallback Potion", "Restores 1 HP", "Common", 1,}); // Provide all members
        return false;
    }

    json parsedJson;
    try {
        configFile >> parsedJson; 
    } catch (json::parse_error& e) {
        message = "Error parsing items config: " + std::string(e.what());
        std::cerr << message << std::endl;
        allItems.push_back({"Fallback Potion", "Restores 1 HP", "Common", 1});
        return false;
    }

    if (!parsedJson.is_array()) {
        message = "Error: Items config JSON is not an array.";
        std::cerr << message << std::endl;
        allItems.push_back({"Fallback Potion", "Restores 1 HP", "Common", 1});
        return false;
    }

    for (const auto& itemEntry : parsedJson) {
        if (!itemEntry.is_object()) {
            std::cerr << "Warning: Found non-object entry in items array. Skipping." << std::endl;
            continue;
        }

        Item newItem;
        bool validEntry = true;

        if (itemEntry.contains("name") && itemEntry["name"].is_string()) {
            newItem.name = itemEntry["name"].get<std::string>();
        } else {
            std::cerr << "Warning: Item entry missing or invalid 'name'. Skipping." << std::endl;
            validEntry = false;
        }

        if (itemEntry.contains("effect") && itemEntry["effect"].is_string()) {
            newItem.effect = itemEntry["effect"].get<std::string>();
        } else {
            if(validEntry) std::cerr << "Warning: Item '" << newItem.name << "' missing or invalid 'effect'." << std::endl;
            else std::cerr << "Warning: Item with missing name also missing 'effect'." << std::endl;
            newItem.effect = "N/A"; 
        }

        if (itemEntry.contains("classification") && itemEntry["classification"].is_string()) {
            newItem.classification = itemEntry["classification"].get<std::string>();
        } else {
            if(validEntry) std::cerr << "Warning: Item '" << newItem.name << "' missing or invalid 'classification'." << std::endl;
            newItem.classification = "Unknown";
        }
        
        if (itemEntry.contains("probability_weight") && itemEntry["probability_weight"].is_number_integer()) {
            newItem.probability_weight = itemEntry["probability_weight"].get<int>();
            if (newItem.probability_weight <= 0) {
                 if(validEntry) std::cerr << "Warning: Item '" << newItem.name << "' has non-positive probability_weight. Setting to 1." << std::endl;
                 newItem.probability_weight = 1; 
            }
        } else {
            if(validEntry) std::cerr << "Warning: Item '" << newItem.name << "' missing or invalid 'probability_weight'." << std::endl;
            newItem.probability_weight = 1; 
        }

        if (validEntry) {
            allItems.push_back(newItem);
        }
    }

    if (allItems.empty()) {
        message = "Warning: No valid items loaded from config. Using fallback.";
        std::cerr << message << std::endl;
        allItems.push_back({"Fallback Potion", "Restores 1 HP", "Common", 1});
        return false; 
    }
    
    return true;
}
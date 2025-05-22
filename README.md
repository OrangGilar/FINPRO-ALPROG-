# Russian Roulette Game with Raylib

This is a simple game implementation using Raylib and C++. The game implements a turn-based system with items and character stats.

## Project Structure

```
src/
├── main.cpp       # Main game logic and UI
├── characters.h   # Character structure definition
├── items.h        # Item structure definition
└── search.h       # Utility functions for searching and sorting
```

## Components Breakdown

### 1. Characters (characters.h)
The `Character` structure defines the basic properties of a game character:
- `name`: Character's name (string)
- `health`: Health points (default: 3)
- `hasKnife`: Flag for knife possession (doubles damage)
- `hasGoggles`: Flag for goggles possession (sees danger)
- `hasEnergyDrink`: Flag for energy drink possession (enables double turns)
- `inventory`: Vector of items the character possesses

### 2. Items (items.h)
The `Item` structure defines game items:
- `name`: Name of the item
- `effect`: Description of what the item does

Available items in the game:
- Health Pot: Restores 2 health points
- Knife: Enables double damage
- Goggles: Allows seeing danger
- Energy Drink: Enables taking two turns

### 3. Search Utilities (search.h)
Contains utility functions for sorting and searching:
- `quickSort`: Implementation of QuickSort algorithm (descending order)
- `binarySearchScore`: Binary search implementation for finding scores

### 4. Main Game (main.cpp)
The main game implementation includes:

#### Game State
- Two characters: Player and AI
- List of available items
- Button definitions for various actions

#### UI Elements
The game includes several interactive buttons:
- Shoot Self
- Get Items
- Skip Turn
- Shoot Enemy

#### Core Functions
- `CheckButton`: Handles button click detection
- `DrawCharacterStatus`: Renders character stats and inventory
- `GetRandomItem`: Randomly selects an item from available items

#### Game Loop
The main game loop handles:
- Window initialization
- Drawing UI elements
- Processing player input
- Updating game state

## Building the Project

The project can be built in two modes:

### Debug Build
```bash
mingw32-make RAYLIB_PATH=C:/raylib/raylib PROJECT_NAME=${fileBasenameNoExtension} OBJS=src/*.cpp BUILD_MODE=DEBUG
```

### Release Build
```bash
mingw32-make RAYLIB_PATH=C:/raylib/raylib PROJECT_NAME=${fileBasenameNoExtension} OBJS=src/*.cpp
```

## Dependencies
- Raylib: Graphics library for game development
- MinGW: Required for compilation on Windows

## Technical Requirements
- Windows OS
- Raylib installed at C:/raylib/raylib
- MinGW installed and configured

## Game Controls
- Use mouse to interact with buttons
- Left-click to activate buttons
- Close window to exit game
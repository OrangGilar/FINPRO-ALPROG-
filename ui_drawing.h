#ifndef UI_DRAWING_H
#define UI_DRAWING_H

#include "raylib.h"
#include "characters.h"

void DrawCharacterStatusUI(const Character& c, int x, int y, bool isTurn);
void DrawGameUI();

#endif // UI_DRAWING_H
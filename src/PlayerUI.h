#ifndef ASTEROID_PLAYER_UI_H_
#define ASTEROID_PLAYER_UI_H_

#include "Player.hpp"

void DrawEnergyBar(Energy &energy, const Rectangle& pos);
void DrawUltraBar(Energy &energy, const Rectangle& pos);
//todo remove this, display by texture instead
void DrawStoredAsteroids(PlayerState &player, const int worldWidth, const int worldHeight);

#endif

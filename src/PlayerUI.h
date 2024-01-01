#ifndef ASTEROID_PLAYER_UI_H_
#define ASTEROID_PLAYER_UI_H_

#include "Player.hpp"

void DrawEnergyBar(Energy& energy, const Vector2& pos, const float maxLength, const float width, const Color& color);

void DrawEnergyPerc(Energy &energy, const int worldWidth, const int worldHeight);

//todo remove this, display by texture instead
void DrawStoredAsteroids(PlayerState &player, const int worldWidth, const int worldHeight);

#endif
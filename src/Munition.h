#ifndef MUNITION_H
#define MUNITION_H

#include "Player.hpp"

struct Shoot
{
  Vector2 position;
  Vector2 direction;
  float radius;
  float speed;
};

void UpdateShoots(std::vector<Shoot>& shoots);
void DrawShoots(const std::vector<Shoot>& shoots);
void FireShoot(PlayerSteer& player);

#endif

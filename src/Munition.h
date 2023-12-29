#ifndef MUNITION_H
#define MUNITION_H

#include "Player.hpp"
#include "EntityComponents.h"

struct Shoot
{
  PhysicsComponent data;
  float time_alive = 0;
};

void UpdateShoots(std::vector<Shoot>& shoots, float dt);
void RemoveOldShoots(std::vector<Shoot>& shoots, float t);
void DrawShoots(const std::vector<Shoot>& shoots);
void FireShoot(const Vector2 &position, const Vector2 &direction, 
               const Vector2 &velocity, const float maxAcceleration, 
               std::vector<Shoot> &shoots);
void FireShoot(const PhysicsComponent& player, const Vector2& direction, float muzzle_velocity, std::vector<Shoot> &shoots);
void FireShootgun(PlayerState& player, const Vector2& direction, float muzzle_velocity, std::vector<Shoot> &shoots);

#endif

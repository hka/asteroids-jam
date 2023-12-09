#ifndef ASTEROID_ENEMY_H
#define ASTEROID_ENEMY_H

#include "EntityComponents.h"

#include "Asteroid.h"

#include <vector>

struct Enemy{
  Vector2 position;
  float radius;
  MovementComponent movement;
};

//update
void UpdateEnemy(Enemy &enemy, const Vector2 &worldBound,
                 const std::vector<Asteroid>& asteroids);

//paint
void PaintEnemy(Enemy& Enemy);

//factory
Enemy CreateEnemy(const Vector2 &worldBound);

#endif
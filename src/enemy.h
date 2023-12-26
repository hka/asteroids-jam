#ifndef ASTEROID_ENEMY_H
#define ASTEROID_ENEMY_H

#include "EntityComponents.h"

#include "Asteroid.h"

#include <vector>

struct Enemy{
  float value = 10;
  PhysicsComponent data;
};

//update
void UpdateEnemy(Enemy &enemy, const Vector2 &worldBound, float dt);

//paint
void PaintEnemy(Enemy& Enemy);

//factory
Enemy CreateEnemy(const Vector2 &worldBound);

#endif

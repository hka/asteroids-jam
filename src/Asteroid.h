#ifndef ASTEROID_ASTEROID_H
#define ASTEROID_ASTEROID_h

#include "EntityComponents.h"

struct Asteroid{
  Vector2 position;
  float radius;
  MovementComponent movement;
};

//update
void UpdateAsteroid(Asteroid &asteroid, const Vector2 &worldBound);

//paint
void PaintAsteroid(Asteroid& asteroid);

//helper
Vector2 getRandomPosOutsideBounds(Rectangle rec);

//factory
Asteroid CreateAsteroid(const Vector2 &worldBound);

#endif
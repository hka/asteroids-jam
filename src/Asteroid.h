#ifndef ASTEROID_ASTEROID_H
#define ASTEROID_ASTEROID_H

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

//factory
Asteroid CreateAsteroid(const Vector2 &worldBound);

#endif

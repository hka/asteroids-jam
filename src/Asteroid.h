#ifndef ASTEROID_ASTEROID_H
#define ASTEROID_ASTEROID_H

#include "EntityComponents.h"

struct Asteroid{
  float value = 1;
  PhysicsComponent data;

  int target = 0;
  Vector2 attract_point;
  int art_ix = 2;
  int type = 0;

  int hp = 0;
};

constexpr const float ASTEROID_MIN_RADIUS = 8.f;
constexpr const float ASTEROID_MAX_RADIUS = 25.f;
constexpr const float ASTEROID_SCALE = 0.01f;

//update
void UpdateAsteroid(Asteroid &asteroid, const Vector2 &worldBound, float dt);

//paint
void PaintAsteroid(Asteroid& asteroid);

//factory
Asteroid CreateAsteroid(const Vector2 &worldBound);
Asteroid CreateAsteroid(const Vector2& pos, int type);

#endif

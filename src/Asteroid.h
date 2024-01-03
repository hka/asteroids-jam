#ifndef ASTEROID_ASTEROID_H
#define ASTEROID_ASTEROID_H

#include <vector>

#include "EntityComponents.h"
#include "Timer.hpp"
#include "Munition.h"

enum State{
  ALIVE,
  KILLED,
  DEAD_BY_COLLISION,
  ABSORBED
};

struct Asteroid{
  float value = 1;
  PhysicsComponent data;

  int target = 0;
  Vector2 attract_point;
  int art_ix = 2;
  int type = 0;

  int hp = 0;
  State state;

  Timer damageTimer;
  bool shouldDamageBlink;

  float shoot_counter = 0;
};

constexpr const float ASTEROID_MIN_RADIUS = 8.f;
constexpr const float ASTEROID_MAX_RADIUS = 25.f;
constexpr const float ASTEROID_SCALE = 0.01f;
constexpr const float ASTEROID_DAMAGE_BLINK_TIME = 0.2f;

//update
void UpdateAsteroid(Asteroid& asteroid, const Vector2& worldBound, std::vector<Shoot> &shoots, float dt);

//Helper
void OnAsteroidSplit(std::vector<Asteroid> &asteroids, const int type, const Vector2 &pos);

// paint
void PaintAsteroid(Asteroid &asteroid);

//factory
Asteroid CreateAsteroid(const Vector2 &worldBound);
Asteroid CreateAsteroid(const Vector2& pos, int type);

#endif

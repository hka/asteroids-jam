#include "Asteroid.h"

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void UpdateAsteroid(Asteroid& asteroid, const Vector2& worldBound){
  UpdateMovement(asteroid.position, asteroid.movement, worldBound);
}

////////////////////////////////////////////////
///         Paint                           ///
///////////////////////////////////////////////
void PaintAsteroid(Asteroid& asteroid){
  DrawCircle(asteroid.position.x, asteroid.position.y, asteroid.radius, BROWN);
}

////////////////////////////////////////////////
///         Factory                         ///
///////////////////////////////////////////////
Asteroid CreateAsteroid(const Vector2& worldBound){
  Asteroid asteroid;

  asteroid.radius = GetRandomValue(ASTEROID_MIN_RADIUS, ASTEROID_MAX_RADIUS);

  asteroid.position = getRandomPosOutsideBounds({0.f,0.f, worldBound.x, worldBound.y}, asteroid.radius);
  asteroid.data.position = asteroid.position;

  const float maxAcceleration = 50.f;
  const float minAcceleration = 15.f;
  const float radiusPercentage = 1.f - ((asteroid.radius / ASTEROID_MAX_RADIUS));

  asteroid.movement.currentAcceleration = (maxAcceleration * radiusPercentage) + minAcceleration;

  asteroid.movement.rotation = (float)GetRandomValue(0, 7);
  return asteroid;
}

Asteroid CreateAsteroid(const Vector2 &pos, float radius){
  Asteroid asteroid;

  asteroid.radius = radius;
  asteroid.position = pos;
  asteroid.data.position = asteroid.position;

  const float maxAcceleration = 50.f;
  const float minAcceleration = 15.f;
  const float radiusPercentage = 1.f - ((asteroid.radius / ASTEROID_MAX_RADIUS));

  asteroid.movement.currentAcceleration = (maxAcceleration * radiusPercentage) + minAcceleration;

  asteroid.movement.rotation = (float)GetRandomValue(0, 7);
  return asteroid;
}

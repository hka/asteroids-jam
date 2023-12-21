#include "Asteroid.h"

#include "globals.h"

#include <raymath.h>

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void UpdateAsteroid(Asteroid& asteroid, const Vector2& worldBound, float dt)
{
  ApplyThrustDrag(asteroid.data);
  UpdatePosition(asteroid.data, worldBound, dt);
}

////////////////////////////////////////////////
///         Paint                           ///
///////////////////////////////////////////////
void PaintAsteroid(Asteroid& asteroid){
  DrawCircle(asteroid.data.position.x, asteroid.data.position.y, asteroid.data.radius, BROWN);
}

////////////////////////////////////////////////
///         Factory                         ///
///////////////////////////////////////////////
Asteroid CreateAsteroid(const Vector2& worldBound){
  Asteroid asteroid;
  float radius = GetRandomValue(ASTEROID_MIN_RADIUS, ASTEROID_MAX_RADIUS);
  asteroid.data.radius = radius;
  asteroid.data.position = getRandomPosOutsideBounds({0.f,0.f, worldBound.x, worldBound.y}, radius);

  asteroid.data.mass = M_PI*radius*radius;
  asteroid.data.drag = 1.2f*radius/ASTEROID_MAX_RADIUS; //controlls terminal velocity should depend on radius maybe
  asteroid.data.thrust = 5000; //todo calculate reasonable values
  std::uniform_real_distribution<> distrib(-1.f, 1.f);
  asteroid.data.orientation = {(float)distrib(RNG),(float)distrib(RNG)};
  asteroid.data.orientation = Vector2Normalize(asteroid.data.orientation);
  
  return asteroid;
}

Asteroid CreateAsteroid(const Vector2 &pos, float radius){
  Asteroid asteroid;

  asteroid.data.radius = radius;
  asteroid.data.position = pos;

  asteroid.data.mass = M_PI * radius * radius;
  asteroid.data.drag = 1.2f * radius / ASTEROID_MAX_RADIUS; // controlls terminal velocity should depend on radius maybe
  asteroid.data.thrust = 5000;                             // todo calculate reasonable values
  std::uniform_real_distribution<> distrib(-1.f, 1.f);
  asteroid.data.orientation = {(float)distrib(RNG), (float)distrib(RNG)};
  asteroid.data.orientation = Vector2Normalize(asteroid.data.orientation);

  return asteroid;
}

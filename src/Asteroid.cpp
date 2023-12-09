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

  const int minRadius = 8.f;
  const int maxRadius = 25.f; 
  asteroid.radius = GetRandomValue(minRadius, maxRadius);

  asteroid.position = getRandomPosOutsideBounds({0.f,0.f, worldBound.x, worldBound.y}, asteroid.radius);

  const float maxAcceleration = 50.f;
  const float minAcceleration = 15.f;
  const float radiusPercentage = 1.f - ((asteroid.radius / maxRadius));

  asteroid.movement.currentAcceleration = (maxAcceleration * radiusPercentage) + minAcceleration; //todo calculate init value

  asteroid.movement.rotation = (float)GetRandomValue(0, 7);
  return asteroid;
}

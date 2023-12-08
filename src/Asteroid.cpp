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
///         helper                           ///
///////////////////////////////////////////////
Vector2 getRandomPosOutsideBounds(Rectangle boundary, float radius){
  Vector2 position;
  int choice = GetRandomValue(1, 4); // Randomly choose one of the four areas
  switch (choice)
  {
  case 1:
    position.x = GetRandomValue((int)boundary.x, (int)boundary.width);
    position.y = -radius;
    break;
  case 2:
    position.x = GetRandomValue((int)boundary.x, (int)boundary.width);
    position.y = boundary.height + radius;
    break;
  case 3:
    position.x = -radius;
    position.y = GetRandomValue((int)boundary.y, (int)boundary.height);
    break;
  case 4:
    position.x = boundary.width + radius;
    position.y = GetRandomValue(boundary.y, (int)boundary.height);
    break;
  }

  return position;
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
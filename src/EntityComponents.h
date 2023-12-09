#ifndef ASTEROID_ENTITY_COMPONENT_H_
#define ASTEROID_ENTITY_COMPONENT_H_

#include "raylib.h"

struct MovementComponent
{
  Vector2 velocity;
  float currentAcceleration;
  float maxAcceleration;
  float accelerationDecrease;

  float rotation;
  float rotationSpeed;
};

void UpdateMovement(Vector2 &position, MovementComponent &movement, const Vector2 &bound);
void UpdateVelocity(const float rotation, Vector2& velocity, const float currentAcceleration);
void UpdatePosition(Vector2 &position, Vector2 &velocity, const Vector2 &bound);

//helper
Vector2 getRandomPosOutsideBounds(Rectangle rec, float radius);

#endif

#include "EntityComponents.h"

#include "raylib.h"
#include "raymath.h"
#include "raylib_operators.h"
#include "helpers.h"

void UpdateMovement(Vector2 &position, MovementComponent &movement, const Vector2 &bound)
{
  UpdateVelocity(movement.rotation, movement.velocity, movement.currentAcceleration);
  UpdatePosition(position, movement.velocity, bound);
}

void UpdateVelocity(const float rotation, Vector2 &velocity, const float acceleration)
{
  Vector2 newVelocity = Vector2Normalize({cosf(rotation), sinf(rotation)});
  newVelocity.x *= acceleration;
  newVelocity.y *= acceleration;
  velocity = newVelocity;
}

void UpdatePosition(Vector2 &position, Vector2& velocity, const Vector2& bound)
{
  float dt = GetFrameTime();
  Vector2 newPos = Vector2Add(position, Vector2Scale(velocity, dt));
  position = mod(newPos + bound, bound);
}

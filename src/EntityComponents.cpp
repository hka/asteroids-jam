#include "EntityComponents.h"

#include "raylib.h"
#include "raymath.h"
#include "raylib_operators.h"
#include "helpers.h"

void UpdatePosition(PhysicsComponent& data, const Vector2& bound)
{

}


void UpdateMovement(Vector2 &position, MovementComponent &movement, const Vector2 &bound)
{
  //UpdateVelocity(movement.rotation, movement.velocity, movement.currentAcceleration);
  UpdateVelocity(movement.force, movement.velocity);
  UpdatePosition(position, movement.velocity, bound);
}

void UpdateVelocity(const float rotation, Vector2 &velocity, const float acceleration)
{
  Vector2 newVelocity = Vector2Normalize({cosf(rotation), sinf(rotation)});
  newVelocity.x *= acceleration;
  newVelocity.y *= acceleration;
  velocity = newVelocity;
}

void UpdateVelocity(Vector2 &force, Vector2 &velocity){
  velocity += force;
  force = {0.f, 0.f};
}

void UpdatePosition(Vector2 &position, Vector2& velocity, const Vector2& bound)
{
  float dt = GetFrameTime();
  Vector2 newPos = Vector2Add(position, Vector2Scale(velocity, dt));
  position = mod(newPos + bound, bound);
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

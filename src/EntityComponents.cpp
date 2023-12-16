#include "EntityComponents.h"

#include "raylib.h"
#include "raymath.h"
#include "raylib_operators.h"
#include "helpers.h"

void ApplyThrustDrag(PhysicsComponent& data)
{
  //when ft and fd balance, we get steady state
  float speed_squared = Vector2LengthSqr(data.velocity);
  Vector2 ft = data.orientation*data.thrust;
  Vector2 velocity_dir = Vector2Normalize(data.velocity);
  Vector2 fd = velocity_dir*data.drag*speed_squared;
  data.force += ft - fd;
}

void UpdatePosition(PhysicsComponent& data, const Vector2& bound, float dt, bool no_limit)
{
  //set acceleration
  data.acceleration = data.mass == 0 ? Vector2{0.f, 0.f} : data.force/data.mass;
  //update velocity
  Vector2 start_vel = data.velocity;
  data.velocity += data.acceleration*dt;


  if(!no_limit)
  {
    if(Vector2Length(data.velocity)*dt > 5)
    {
      data.velocity = data.velocity/Vector2Length(data.velocity) * 5.f/dt;
    }
  }

  //and position
  data.position += data.velocity*dt;
  data.position = mod(data.position + bound, bound);
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

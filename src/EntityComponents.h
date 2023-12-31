#ifndef ASTEROID_ENTITY_COMPONENT_H_
#define ASTEROID_ENTITY_COMPONENT_H_

#include "raylib.h"

struct MovementComponent
{
  Vector2 velocity;
  Vector2 direction;
  Vector2 force;
  
  float currentAcceleration;
  float maxAcceleration;
  float accelerationDecrease;

  float rotation;
  float rotationSpeed;
};

struct PhysicsComponent
{
  Vector2 position = {0,0};

  float drag = 0; //constant for balancing acceleration
  float mass = 0;
  float thrust = 0;
  float radius = 0;

  Vector2 orientation = {0,0};
  Vector2 force = {0,0};
  Vector2 acceleration = {0,0};
  Vector2 velocity = {0,0};
};

void UpdatePosition(PhysicsComponent& data, const Vector2& bound, float dt, bool no_limit = false);
void ApplyThrustDrag(PhysicsComponent& data);



void UpdateMovement(Vector2 &position, MovementComponent &movement, const Vector2 &bound);
void UpdateVelocity(const float rotation, Vector2& velocity, const float currentAcceleration);
void UpdateVelocity(Vector2& force, Vector2& velocity);
void UpdatePosition(Vector2 &position, Vector2 &velocity, const Vector2 &bound);

//helper
Vector2 getRandomPosOutsideBounds(Rectangle rec, float radius);

#endif

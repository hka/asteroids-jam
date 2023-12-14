#ifndef ASTEROID_PLAYER_H_
#define ASTEROID_PLAYER_H_

#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "Timer.hpp"
#include "EntityComponents.h"

struct Shoot;

struct Ball{
  Vector2 position;
  float radius;
  float speed;
};

struct SuckAttack{
  bool isOngoing;
  float lineLength;
  Vector2 linesEnd[2];
  std::vector<Ball> balls;
  Timer addBallTimer;

};

struct GunAttack{
  Vector2 direction;
  Timer cooldownTimer;
  float cooldownDuration;
};

struct PlayerSteer
{
  Vector2 position;

  MovementComponent movement;
  SuckAttack suckAttack;
  GunAttack gun;

  PhysicsComponent data;
};

// update
void update(PlayerSteer &player, const Vector2 &worldBound, std::vector<Shoot>& shoots);

//input
void RotateShip(Vector2& direction, float rotationSpeed);
void accelerate(MovementComponent &movement);
void suckAttack(const Vector2 &position, const float rotation, SuckAttack &suckAttack);
void gunUpdate(const PlayerSteer& player, GunAttack& gun, std::vector<Shoot>& shoots);

//helper
void rotateTriangle(Vector2 (&v)[3], const float angle);
void rotateTriangle(Vector2 (&v)[3], const float angle, const Vector2 &center);
void rotateTriangle(Vector2 (&v)[3], const Vector2 &direction, const Vector2 &center);
Vector2 RandomPositionBetweenPoints(Vector2 point1, Vector2 point2);
void moveBallTowardsPoint(Ball &ball, Vector2 targetPoint);

//paint
void DrawShip(const PlayerSteer &player);
void DrawGun(const PlayerSteer &player);

//factory
PlayerSteer createPlayer(Vector2 startPos);

#endif

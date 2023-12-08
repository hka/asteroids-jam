#ifndef _Player
#define _Player

#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "Timer.hpp"
#include "EntityComponents.h"

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

struct PlayerSteer
{
  Vector2 position;

  Vector2 gunDirection;

  MovementComponent movement;
  SuckAttack suckAttack;
};

// update
void update(PlayerSteer &player, const Vector2 &worldBound);

//input
void rotateShip(MovementComponent& movement);
void accelerate(MovementComponent &movement);
void suckAttack(const Vector2 &position, const float rotation, SuckAttack &suckAttack);

//helper
void rotateTriangle(Vector2 (&v)[3], const float angle);
void rotateTriangle(Vector2 (&v)[3], const float angle, const Vector2 &center);
Vector2 RandomPositionBetweenPoints(Vector2 point1, Vector2 point2);
void moveBallTowardsPoint(Ball &ball, Vector2 targetPoint);

//paint
void DrawShip(const PlayerSteer &player);
void DrawGun(const PlayerSteer &player);

//factory
PlayerSteer createPlayer(Vector2 startPos);

#endif

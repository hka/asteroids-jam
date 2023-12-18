#ifndef ASTEROID_PLAYER_H_
#define ASTEROID_PLAYER_H_

#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "Timer.hpp"
#include "EntityComponents.h"
#include "Asteroid.h"

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

struct PlayerState
{
  float score = 0;
  bool alive = true;

  MovementComponent movement;
  SuckAttack suckAttack;
  GunAttack gun;

  PhysicsComponent data;

  void OnHit();
};

// update
void update(PlayerState &player, const Vector2 &worldBound, std::vector<Shoot>& shoots, float dt);

//input
void UpdatePlayerInput(PhysicsComponent& data, float dt);
void suckAttack(const Vector2 &position, const Vector2& rotation, SuckAttack &suckAttack);
void PaintAttractAsteroids(PlayerState& player, std::vector<Asteroid>& asteroids, std::vector<float>& player_asteroid_distance);
void gunUpdate(const PlayerState& player, GunAttack& gun, std::vector<Shoot>& shoots);

//helper
void rotateTriangle(Vector2 (&v)[3], const float angle);
void rotateTriangle(Vector2 (&v)[3], const float angle, const Vector2 &center);
void rotateTriangle(Vector2 (&v)[3], const Vector2 &direction, const Vector2 &center);
Vector2 RandomPositionBetweenPoints(Vector2 point1, Vector2 point2);
void moveBallTowardsPoint(Ball &ball, Vector2 targetPoint);

//paint
void DrawShip(const PlayerState &player);
void DrawGun(const PlayerState &player);

//factory
PlayerState createPlayer(Vector2 startPos);

#endif

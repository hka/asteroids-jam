#ifndef _Player
#define _Player

#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "Timer.hpp"

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
    float rotation;
    float rotationSpeed;

    Vector2 velocity;
    float currentAcceleration;
    float maxAcceleration;
    float accelerationDecrease;

    SuckAttack suckAttack;
};

void rotateShip(PlayerSteer& player);
void accelerate(PlayerSteer& player);
void rotateTriangle(Vector2 (&v)[3], const float angle);
void updateVelocity(const float rotation, Vector2& velocity, const float acceleration);
void updatePosition(Vector2& v, Vector2& velocity, const float dt);
void suckAttack(Vector2& position, const float rotation, SuckAttack& suckAttack);
Vector2 RandomPositionBetweenPoints(Vector2 point1, Vector2 point2);
void moveBallTowardsPoint(Ball &ball, Vector2 targetPoint);

PlayerSteer createPlayer(Vector2 startPos);
void update(PlayerSteer& player);

void rotateShip(PlayerSteer& player);

void accelerate(PlayerSteer& player);

void rotateTriangle(Vector2 (&v)[3], const float angle);

void updateVelocity(const float rotation, Vector2& velocity, const float acceleration);

void updatePosition(Vector2 (&v)[3], Vector2& velocity, const float dt);

void DrawShip(const PlayerSteer& player);

// Function to find a random position between two points
Vector2 RandomPositionBetweenPoints(Vector2 point1, Vector2 point2);

inline void moveBallTowardsPoint(Ball &ball, Vector2 targetPoint);

void suckAttack(Vector2& startPoint, const float rotation, SuckAttack& suckAttack);

#endif

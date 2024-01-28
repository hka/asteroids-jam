#ifndef ASTEROID_PLAYER_H_
#define ASTEROID_PLAYER_H_

#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "Timer.hpp"
#include "EntityComponents.h"
#include "Laser.h"
#include "Asteroid.h"

constexpr int MAX_STORED_ASTEROIDS = 5;
constexpr float SUCK_DELAY = 0.2f;

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

struct Energy{
  float value = 0.f;
  float maxValue = 100.f;
};

struct EnergyShield{
  Energy energy;
  bool canRegain;
  float maxDuration;
  Timer timeSinceLastDamage;
  float durationBeforeCanRegainAfterDamage;
  float missingEnergy;
  Timer regainTimer;
};

const float DASH_ENERGY_COST = 2.f;
const float DASH_DISTANCE = 0.15f;
const float ULTRA_ENERGY_COST = 30.f;

struct PlayerState
{
  float score = 0;
  bool alive = true;

  MovementComponent movement;
  SuckAttack suckAttack;
  GunAttack gun;
  Laser laser;
  EnergyShield shield;

  int storedAsteroids = 0;
  Timer suckDelayTimer;

  bool dash_in_progress = false;
  
  PhysicsComponent data;

  void OnHit(int damage);
};

// update
void update(PlayerState &player, const Vector2 &worldBound, std::vector<Shoot>& shoots, float dt);
void UpdateEnergy(Energy& energy, const float value);
void UpdateEnergyShield(EnergyShield& shield);
void RegainEnergyShield(EnergyShield& shield);

//input
void UpdatePlayerInput(PlayerState& player, float dt);
void suckAttack(const Vector2 &position, const Vector2& rotation, SuckAttack &suckAttack);
void PaintAttractAsteroids(PlayerState& player, std::vector<Asteroid>& asteroids, std::vector<float>& player_asteroid_distance);
void AttractAsteroids(PlayerState& player, std::vector<Asteroid>& asteroids);
void gunUpdate(PlayerState& player, GunAttack& gun, std::vector<Shoot>& shoots);
void laserUpdate(PlayerState &player);

//helper
void rotateTriangle(Vector2 (&v)[3], const float angle);
void rotateTriangle(Vector2 (&v)[3], const float angle, const Vector2 &center);
void rotateTriangle(Vector2 (&v)[3], const Vector2 &direction, const Vector2 &center);
Vector2 RandomPositionBetweenPoints(Vector2 point1, Vector2 point2);
void moveBallTowardsPoint(Ball &ball, Vector2 targetPoint);
bool hasEnoughEnergy(const Energy& energy, const float cost);

//paint
void DrawShip(const PlayerState &player);
void DrawGun(const PlayerState &player);

//factory
PlayerState createPlayer(Vector2 startPos);

#endif

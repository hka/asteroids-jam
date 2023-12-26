#ifndef ASTEROID_LASER_H_
#define ASTEROID_LASER_H_

#include <vector>

#include "raylib.h"
#include "Asteroid.h"
#include "Timer.hpp"
#include "enemy.h"

struct Particle{
  Color color;
  float radius;
  float distance;
  float speed;
};

struct Laser
{
  bool isOngoing;
  Vector2 start;
  Vector2 end;
  Vector2 direction;
  float length;
  float maxLength;
  float growRate;
  float width;
  
  Ray ray;
  bool isHitting;

  Timer spawnParticleTimer;
  std::vector<Particle> particles;
};

constexpr const std::size_t MAXIMUM_PARTICLES = 30;

void OnStart(Laser& laser, const Vector2& direction, const Vector2& startPoint);
void Update(Laser &laser, const Vector2 &direction, const Vector2 &origin);
void Clear(Laser& laser);
void DrawLaser(Laser& laser);

Particle CreateLaserParticle(Color color);
void UpdateParticles(Laser& laser);
void UpdateParticle(Particle &p, Laser &laser);
void DrawParticles(std::vector<Particle> &particles, const Vector2 &laserStart, const Vector2 &laserDirection);

#endif
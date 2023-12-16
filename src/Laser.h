#ifndef ASTEROID_LASER_H_
#define ASTEROID_LASER_H_

#include <vector>

#include "raylib.h"
#include "Asteroid.h"
#include "Timer.hpp"

struct Particle{
  Color color;
  Vector2 position;
  float lifeTime;
  float radius;
  float distance;

  //////
  float speed;
  float amplitude;
  float frequency;
  float angle;
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
  std::vector<Particle> particlesRight;
  std::vector<Particle> particlesLeft;
};

void OnStart(Laser& laser, const Vector2& direction, const Vector2& startPoint);
void Update(Laser &laser, const Vector2 &direction, const Vector2 &origin);
void CheckCollision(Laser& laser, std::vector<Asteroid> asteroids);
void Clear(Laser& laser);
void DrawLaser(Laser& laser);

Particle CreateLaserParticle(Laser& laser, Color color);
void UpdateParticles(Laser& laser);
void UpdateParticle(Particle &p, Laser &laser, bool leftToRight);
void DrawParticles(std::vector<Particle>& particles);

#endif
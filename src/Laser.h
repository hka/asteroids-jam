#ifndef ASTEROID_LASER_H_
#define ASTEROID_LASER_H_

#include <vector>

#include "raylib.h"
#include "Asteroid.h"

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
};

constexpr const std::size_t MAXIMUM_PARTICLES = 30;
constexpr const float LASER_MAX_LENGTH = 400;
constexpr const float LASER_HEIGHT = 30;

void CreateLaserTexture();
void OnStart(Laser& laser, const Vector2& direction, const Vector2& startPoint);
void Update(Laser &laser, const Vector2 &direction, const Vector2 &origin);
void Clear(Laser& laser);
void DrawLaser(Laser& laser);
void NoiseLaser();
void RenderNoiseLaser();
void DistortLaser();
void RenderDistortLaser();

#endif

#ifndef ASTEROID_LASER_H_
#define ASTEROID_LASER_H_

#include <vector>

#include "raylib.h"
#include "Asteroid.h"
#include "Timer.hpp"

struct LaserGraphics
{
  std::vector<Color> laserOriginalPixels;
  std::vector<Color> noiseMap;
  std::vector<Color> distortionMap;
};

struct Laser
{
  bool isOngoing;
  Timer timer;
  float duration;

  Vector2 start;
  Vector2 end;
  Vector2 direction;
  float length;
  float maxLength;
  float growRate;
  float width;
  
  Ray ray;
  bool isHitting;

  float noiseOffset;

  LaserGraphics graphics;
};

constexpr const float LASER_MAX_LENGTH = 800;
constexpr const float LASER_HEIGHT = 80;

Laser createLaser();
void CreateLaserTexture();
LaserGraphics createLaserGraphics();

void OnStart(Laser& laser, const Vector2& direction, const Vector2& startPoint);
void Update(Laser &laser, const Vector2 &direction, const Vector2 &origin);
void Clear(Laser& laser);
void DrawLaser(Laser& laser);
void NoiseLaserWeb(float& noiseOffset, LaserGraphics& graphics);
void DistortLaserWeb(LaserGraphics& graphics);

#endif

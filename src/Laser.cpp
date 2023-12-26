#include "Laser.h"

#include "raymath.h"

//Debug
#include <iostream>

void OnStart(Laser &laser, const Vector2 &direction, const Vector2 &origin){
  laser.isOngoing = true;
  laser.direction = direction;
  laser.start = origin;
  laser.length = 0.f;
  laser.maxLength = 200.f;
  laser.growRate = 800.f;

  laser.width = 10.f;

  laser.spawnParticleTimer.start();
}

void Update(Laser &laser, const Vector2 &direction, const Vector2 &origin)
{
  laser.start = {
      origin.x + (direction.x * 15.f),
      origin.y + (direction.y * 15.f),
  }; 
  laser.direction = direction;

  laser.ray.direction = {laser.direction.x, laser.direction.y, 0.f};
  laser.ray.position = {laser.start.x, laser.start.y, 0.f};

  laser.end = {
    laser.start.x + (laser.direction.x * laser.length),
    laser.start.y + (laser.direction.y * laser.length)
  };

  if(laser.length < laser.maxLength)
  {
    laser.length += (laser.growRate * GetFrameTime());
  }

  if(laser.spawnParticleTimer.getElapsed() > 0.25f && laser.particles.size() < MAXIMUM_PARTICLES){
    Color particleColor;
    int chance = GetRandomValue(0, 100);
    if(chance > 50){
      particleColor = { 255, 0, 0, 255 };
    }else{
      particleColor = {255, 255, 255, 255};
    }
    laser.particles.push_back(CreateLaserParticle(particleColor));
    laser.spawnParticleTimer.start();
  }

  UpdateParticles(laser);
}

void Clear(Laser &laser)
{
  laser.isOngoing = false;
  laser.particles.clear();
}

void DrawLaser(Laser &laser){

  BeginBlendMode(BLEND_SUBTRACT_COLORS | BLEND_ADDITIVE | BLEND_ALPHA);
    DrawParticles(laser.particles, laser.start, laser.direction);
  EndBlendMode();

  float height = laser.length;
  unsigned char value = (unsigned char)50;
  Color color = {value, 0, 0, value};
  int steps = 10;
  unsigned char targetValue = (unsigned char)255;
  unsigned char f = (targetValue - value) / steps;
  float width = laser.width;

  BeginBlendMode(BLEND_ALPHA_PREMULTIPLY | BLEND_ADDITIVE);
    for (int i = 0; i < steps; ++i){
      DrawLineEx(laser.start, laser.end, width, color);
      color.a += f;
      color.r += f;
      width *= 0.5f;
    }
  EndBlendMode();
}

Particle CreateLaserParticle(Color color){
  Particle p;
  p.color = color;
  p.distance = 0.f;
  p.radius = 1.0f;
  p.speed = 200.f;
  return p;
}

void UpdateParticles(Laser &laser){
  for(Particle& p : laser.particles){
    UpdateParticle(p, laser);
  }
}

void UpdateParticle(Particle &p, Laser &laser){
  p.distance += p.speed * GetFrameTime();

  if(p.distance >= (laser.length - 5.f)){
    p.distance = 0.f;
  }
}

void DrawParticles(std::vector<Particle> &particles, const Vector2 &laserStart, const Vector2& laserDirection)
{
  float startOffset = 5.f;
  for(const Particle& p : particles){
    Vector2 position = {
      laserStart.x + (laserDirection.x * p.distance  + (startOffset * laserDirection.x)),
      laserStart.y + (laserDirection.y * p.distance  + (startOffset * laserDirection.y))
    };
    DrawCircleV(position, p.radius, p.color);
  }
}

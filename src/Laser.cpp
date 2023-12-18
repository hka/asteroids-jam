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
      particleColor = { 255,
                        0,
                        0,
                        255 };
    }else{
      particleColor = {255,
                       255,
                       255,
                       255};
    }
    laser.particles.push_back(CreateLaserParticle(particleColor));
    laser.spawnParticleTimer.start();
  }

  UpdateParticles(laser);
}

void CheckCollision(Laser &laser, std::vector<Asteroid> asteroids){
  bool isHitting = false;
  for(const Asteroid& asteroid : asteroids){
    Vector3 asteroidPos = {asteroid.data.position.x, asteroid.data.position.y, 0.f};
    RayCollision col = GetRayCollisionSphere(laser.ray, asteroidPos, asteroid.data.radius);
    
    if(col.hit && col.distance <= laser.maxLength && col.distance > 0.f){
      isHitting = true;
      if (laser.length > col.distance)
      {
        laser.length = col.distance;
      }
    }
  }

  laser.isHitting = isHitting;
}

void Clear(Laser &laser)
{
  laser.isOngoing = false;
  laser.particles.clear();
}

void print(int i)
{
  switch (i)
  {
  case BLEND_ADDITIVE:
    std::cout << "ADDITIVE" << '\n';
    break;
  case BLEND_ADD_COLORS:
    std::cout << "ADD_COLORS" << '\n';
    break;
  case BLEND_ALPHA:
    std::cout << "ALPHA" << '\n';
    break;
  case BLEND_ALPHA_PREMULTIPLY:
    std::cout << "ALPHA_PREMULTIPLY" << '\n';
    break;
  case BLEND_MULTIPLIED:
    std::cout << "BLEND_MULTIPLIED" << '\n';
    break;
  case BLEND_SUBTRACT_COLORS:
    std::cout << "SUBTRACT_COLORS" << '\n';
    break;
  }
}

void DrawLaser(Laser &laser){
  Vector2 p1 = laser.start;
  Vector2 p2 = laser.end;

  Vector2 p3 = {
      p1.x + (laser.direction.x * laser.length * 0.98f),
      p1.y + (laser.direction.y * laser.length * 0.98f)
    };

  int blendModes[6] = {
      BLEND_ADDITIVE, BLEND_ADD_COLORS, BLEND_ALPHA,
      BLEND_ALPHA_PREMULTIPLY, BLEND_MULTIPLIED,
      BLEND_SUBTRACT_COLORS};
  static int mode = 0;

  if (IsKeyPressed(KEY_LEFT))
  {
    mode = mode - 1 < 0 ? 5 : mode - 1;
    print(mode);
  }
  else if (IsKeyPressed(KEY_RIGHT))
  {
    mode = mode + 1 > 4 ? 0 : mode + 1;
    print(mode);
  }

  static unsigned char a = 0;
  a += (unsigned char)1;
  if(a > (unsigned char) 100){
    a = (unsigned char)0;
  }
  Color r0 = {255, 0,0, (unsigned char)(200 - a)};
  Color r1 = {200, 0, 0, (unsigned char)(155 - a)};
  Color r2 = {155, 0, 0, (unsigned char) 100 };
  Color r3 = {55, 0, 0, (unsigned char) 50 };

  BeginBlendMode(BLEND_SUBTRACT_COLORS | BLEND_ADDITIVE | BLEND_ALPHA);
    DrawParticles(laser.particles, laser.start, laser.direction);
  EndBlendMode();

  BeginBlendMode(BLEND_ALPHA_PREMULTIPLY | BLEND_ADDITIVE);
    DrawLineEx(p1, p2, laser.width / 6.f, r0);
    DrawLineEx(p1, p2, laser.width / 3.f, r1);
    DrawLineEx(p1, p2, laser.width / 2.f, r2);
    DrawLineEx(p1, p2, laser.width, r3);
  EndBlendMode();

}

Particle CreateLaserParticle(Color color){
  Particle p;
  p.color = color;
  p.distance = 0.f;
  p.radius = 2.f;
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

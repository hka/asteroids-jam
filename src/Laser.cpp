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
  laser.growRate = 500.f;

  laser.width = 5.f;

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

  if(laser.spawnParticleTimer.getElapsed() > 0.1f){
    laser.particlesRight.push_back(CreateLaserParticle(laser, YELLOW));
    laser.particlesLeft.push_back(CreateLaserParticle(laser, WHITE));
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
  laser.particlesRight.clear();
  laser.particlesLeft.clear();
}

void DrawLaser(Laser &laser){
  Vector2 p1 = laser.start;
  Vector2 p2 = laser.end;

  Vector2 p3 = {
      p1.x + (laser.direction.x * laser.length * 0.98f),
      p1.y + (laser.direction.y * laser.length * 0.98f)
    };
  DrawLineEx(p1, p2, laser.width, RED);
  DrawLineEx(p1, p3, laser.width / 5.f, WHITE);
  if(laser.isHitting){
    BeginBlendMode(BLEND_SUBTRACT_COLORS | BLEND_ADDITIVE);
      
    EndBlendMode();
  }

  DrawParticles(laser.particlesRight);
  DrawParticles(laser.particlesLeft);
}

Particle CreateLaserParticle(Laser &laser, Color color)
{
  Particle particle;
  particle.position = {laser.start.x, laser.start.y};
  particle.color = color;
  particle.lifeTime = 0.f;
  particle.radius = 2.f;
  particle.distance = 0.f;

  particle.speed = 100.f;
  particle.amplitude = 2.f;
  particle.frequency = 10.f;

  particle.angle = 0.f;
  return particle;
}

void UpdateParticles(Laser &laser)
{
  Vector2 perpendicular = {-laser.direction.y, laser.direction.x};
  float particleLifeTime = 1.90f;
  for(std::size_t i = 0; i < laser.particlesRight.size(); ++i){
    laser.particlesRight[i].lifeTime += GetFrameTime();
    if (laser.particlesRight[i].lifeTime > particleLifeTime)
    {
      std::swap(laser.particlesRight[i], laser.particlesRight.back());
      laser.particlesRight.pop_back();
      --i;
      continue;
    }
    UpdateParticle(laser.particlesRight[i], laser, true);
  }
  
  perpendicular = {laser.direction.y, -laser.direction.x};
  for (std::size_t i = 0; i < laser.particlesLeft.size(); ++i)
  {
    laser.particlesLeft[i].lifeTime += GetFrameTime();
    if (laser.particlesLeft[i].lifeTime > particleLifeTime - 0.05f)
    {
      std::swap(laser.particlesLeft[i], laser.particlesLeft.back());
      laser.particlesLeft.pop_back();
      --i;
      continue;
    }

    UpdateParticle(laser.particlesLeft[i], laser, false);
  }
}

void UpdateParticle(Particle &particle, Laser &laser, bool leftToRight)
{
  Vector2 direction = laser.direction;
  float radius = laser.width * 0.80f;

  // Move in the direction of travel
  particle.position.x += direction.x * particle.speed * GetFrameTime();
  particle.position.y += direction.y * particle.speed * GetFrameTime();

  // Update the oscillation angle based on the speed and radius
  particle.angle += (leftToRight ? (particle.speed / radius) : -(particle.speed / radius)) * GetFrameTime(); 

  // Calculate the offset for oscillation
  Vector2 offset = {radius * cos(particle.angle), radius * sin(particle.angle)};

  Vector2 perpendicular = leftToRight ? Vector2{direction.y, -direction.x} : Vector2{-direction.y, direction.x}; 

  Vector2 oscillation = {perpendicular.x * offset.x - perpendicular.y * offset.y,
                         perpendicular.x * offset.y + perpendicular.y * offset.x};

  // Update position with oscillation
  particle.position.x += oscillation.x;
  particle.position.y += oscillation.y;
}

/*
om du gör det med vektorer kan du ex göra följande för en partikel som snurar runt vektorn
om lasern börjar i p0 och pekar i d0
låt d1 var vinkelrät mot d0 (dvs ex d1.x=d0.y, d1.y=-d0.x)
låt p1 = p0+d0A; där A är hur långt från starten av lasern som partikeln ska snurra
låt p2 = p1+d1B; där B är avstånd från lasern
vill du att den ska rotera med radie R så sätt att om B=abs(B)>R?-B:B;
*/

void DrawParticles(std::vector<Particle> &particles)
{
  for(const Particle& p : particles){
    DrawCircleV(p.position, p.radius, p.color);
  }
}

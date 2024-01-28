#include "Collision.h"

#include <stdio.h>

#include "raylib_operators.h"

float handleCollision(std::vector<Enemy> &enemies, std::vector<Shoot> &playerBullets){
  float value_hit = 0;
  for(std::size_t i = 0; i < enemies.size(); ++i){
    Enemy e = enemies[i];
    for(std::size_t j = 0; j < playerBullets.size(); ++j){
      Shoot bullet = playerBullets[j];

      if(CheckCollisionCircles(e.data.position, e.data.radius, bullet.data.position, bullet.data.radius)){
        value_hit += enemies[i].value;
        enemies[i] = enemies[enemies.size() - 1];
        enemies.pop_back();

        playerBullets[j] = playerBullets[playerBullets.size() - 1];
        playerBullets.pop_back(); 

        --i;
        --j;
      }
    }
  }
  return value_hit;
}

float handleCollision(std::vector<Asteroid> &asteroids, std::vector<Shoot> &playerBullets){
  float value_hit = 0;
  for (std::size_t i = 0; i < asteroids.size(); ++i)
  {
    for (std::size_t j = 0; j < playerBullets.size(); ++j)
    {
      Shoot bullet = playerBullets[j];

      if (CheckCollisionCircles(asteroids[i].data.position, asteroids[i].data.radius, bullet.data.position, bullet.data.radius))
      {
        value_hit += asteroids[i].value;
        
        asteroids[i].shouldDamageBlink = true;
        asteroids[i].damageTimer.start();
        asteroids[i].hp -= 1;
        if(asteroids[i].hp <= 0){
          asteroids[i].state = KILLED;
        }

        playerBullets[j] = playerBullets[playerBullets.size() - 1];
        playerBullets.pop_back();

        --j;
      }
    }
  }
  return value_hit;
}

void handleCollision(PlayerState &player, std::vector<Shoot> &bullets)
{
  for(std::size_t j = 0; j < bullets.size(); ++j)
  {
    Shoot bullet = bullets[j];

    if(CheckCollisionCircles(player.data.position, player.data.radius, bullet.data.position, bullet.data.radius)){

      player.OnHit(bullet.damage);

      bullets[j] = bullets[bullets.size() - 1];
      bullets.pop_back();
      --j;
    }
  }
}

void handleCollision(PlayerState &player, std::vector<Asteroid> &asteroids)
{
  for(std::size_t j = 0; j < asteroids.size(); ++j)
  {
    Asteroid a = asteroids[j];

    if(CheckCollisionCircles(player.data.position, player.data.radius, a.data.position, a.data.radius))
    {
      //check if absorbing and we got hit in the front
      Vector2 dir = Vector2Normalize(a.data.position - player.data.position);
      //all in front
      if(a.type == 1
         && std::abs(dot(dir,player.data.orientation)) > 0.7
         && player.suckAttack.isOngoing)
      {
        player.storedAsteroids = std::min(MAX_STORED_ASTEROIDS, player.storedAsteroids + 1);
        asteroids[j].state = ABSORBED;
      }
      else
      {
        int damage = 0;
        float maxShield = player.shield.energy.maxValue;
        switch (a.type)
        {
          case 1:
            damage = maxShield / 4;
            break;
          case 2:
            damage = maxShield / 2;
            break;
          case 3:
            damage = maxShield;
            break;
          default:
            const char* c = "Default damage value";
            TraceLog(LOG_INFO, c);
        }
        
        player.OnHit(damage);

        asteroids[j].state = DEAD_BY_COLLISION;
      }
    }
  }
  //todo
}

float HandleLaserCollision(Laser &laser, std::vector<Asteroid> &asteroids)
{
  bool isHitting = false;
  float value_hit = 0.f;
  for (std::size_t i = 0; i < asteroids.size(); ++i)
  {
    Vector3 asteroidPos = {asteroids[i].data.position.x, asteroids[i].data.position.y, 0.f};
    RayCollision col = GetRayCollisionSphere(laser.ray, asteroidPos, asteroids[i].data.radius);

    if (col.hit && col.distance <= laser.maxLength && col.distance > 0.f)
    {
      isHitting = true;
      if (laser.length > col.distance)
      {
        laser.length = col.distance;
      }

      value_hit += asteroids[i].value;
      asteroids[i].state = KILLED;
    }
  }

  laser.isHitting = isHitting;
  return value_hit;
}

float HandleLaserCollision(Laser &laser, std::vector<Enemy> &enemies)
{
  bool isHitting = false;
  float value_hit = 0.f;
  for(std::size_t i = 0; i < enemies.size(); ++i){
    Vector3 enemyPos = {enemies[i].data.position.x, enemies[i].data.position.y, 0.f};
    RayCollision col = GetRayCollisionSphere(laser.ray, enemyPos, enemies[i].data.radius);

    if(col.hit && col.distance <= laser.maxLength && col.distance >0.f){
      isHitting = true;
      if (laser.length > col.distance)
      {
        laser.length = col.distance;
      }

      value_hit += enemies[i].value;
      std::swap(enemies[i], enemies[enemies.size() - 1]);
      enemies.pop_back();
      --i;
    }
  }

  laser.isHitting = isHitting;
  return value_hit;
}

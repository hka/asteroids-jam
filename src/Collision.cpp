#include "Collision.h"

#include <stdio.h>

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
    Asteroid asteroid = asteroids[i];
    for (std::size_t j = 0; j < playerBullets.size(); ++j)
    {
      Shoot bullet = playerBullets[j];

      if (CheckCollisionCircles(asteroid.data.position, asteroid.data.radius, bullet.data.position, bullet.data.radius))
      {
        value_hit += asteroids[i].value;

        onAsteroidSplit(asteroids, asteroids[i]);

        asteroids[i] = asteroids[asteroids.size() - 1];
        asteroids.pop_back();

        playerBullets[j] = playerBullets[playerBullets.size() - 1];
        playerBullets.pop_back();

        --i;
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

      player.OnHit();

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

    if(a.target == 0 && CheckCollisionCircles(player.data.position, player.data.radius, a.data.position, a.data.radius)){

      player.OnHit();

      asteroids[j] = asteroids[asteroids.size() - 1];
      asteroids.pop_back();
      --j;
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

      onAsteroidSplit(asteroids, asteroids[i]);

      value_hit += asteroids[i].value;
      std::swap(asteroids[i], asteroids[asteroids.size() - 1]);
      asteroids.pop_back();
      --i;
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

void onAsteroidSplit(std::vector<Asteroid> &asteroids, const Asteroid &asteroid)
{
  Vector2 pos = asteroid.data.position;
  if(asteroid.type == 1)
  {
    //nothing, just destroyed
  }
  else if(asteroid.type == 2)
  {
    for(size_t ii = 0; ii < 5; ++ii)
    {
      asteroids.push_back(CreateAsteroid(pos, 1));
    }
  }
  else if(asteroid.type == 3)
  {
    int count = GetRandomValue(2, 3);
    for(int ii = 0; ii < count; ++ii)
    {
      asteroids.push_back(CreateAsteroid(pos, 2));
    }
  }
}

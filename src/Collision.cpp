#include "Collision.h"

void handleCollision(std::vector<Enemy> &enemies, std::vector<Shoot> &playerBullets){
  for(std::size_t i = 0; i < enemies.size(); ++i){
    Enemy e = enemies[i];
    for(std::size_t j = 0; j < playerBullets.size(); ++j){
      Shoot bullet = playerBullets[j];

      if(CheckCollisionCircles(e.position, e.radius, bullet.position, bullet.radius)){
        enemies[i] = enemies[enemies.size() - 1];
        enemies.pop_back();

        playerBullets[j] = playerBullets[playerBullets.size() - 1];
        playerBullets.pop_back(); 

        --i;
        --j;
      }

    }
  }
}

void handleCollision(std::vector<Asteroid> &asteroids, std::vector<Shoot> &playerBullets){
  for (std::size_t i = 0; i < asteroids.size(); ++i)
  {
    Asteroid asteroid = asteroids[i];
    for (std::size_t j = 0; j < playerBullets.size(); ++j)
    {
      Shoot bullet = playerBullets[j];

      if (CheckCollisionCircles(asteroid.data.position, asteroid.data.radius, bullet.position, bullet.radius))
      {
        asteroids[i] = asteroids[asteroids.size() - 1];
        asteroids.pop_back();

        float splitRadius = asteroid.data.radius / 1.5f;
        if(splitRadius >= ASTEROID_MIN_RADIUS){
          asteroids.push_back(CreateAsteroid(asteroid.data.position, splitRadius));
          asteroids.push_back(CreateAsteroid(asteroid.data.position, splitRadius));
        }        

        playerBullets[j] = playerBullets[playerBullets.size() - 1];
        playerBullets.pop_back();

        --i;
        --j;
      }
    }
  }
}

void handleCollision(PlayerSteer &player, std::vector<Shoot> &enemyBullets){
  //todo
}

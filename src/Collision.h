#ifndef ASTEROID_COLLISION_H_
#define ASTEROID_COLLISION_H_

#include <vector>

#include "enemy.h"
#include "Munition.h"
#include "Player.hpp"
#include "Asteroid.h"

float handleCollision(std::vector<Enemy>& enemies, std::vector<Shoot>& playerBullets);
float handleCollision(std::vector<Asteroid> &asteroids, std::vector<Shoot> &playerBullets);
void handleCollision(PlayerState& player, std::vector<Shoot>& enemyBullets);
void handleCollision(PlayerState &player, std::vector<Asteroid> &asteroids);

#endif

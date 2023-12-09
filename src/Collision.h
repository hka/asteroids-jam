#ifndef ASTEROID_COLLISION_H_
#define ASTEROID_COLLISION_H_

#include <vector>

#include "enemy.h"
#include "Munition.h"
#include "Player.hpp"
#include "Asteroid.h"

void handleCollision(std::vector<Enemy>& enemies, std::vector<Shoot>& playerBullets);
void handleCollision(std::vector<Asteroid> &asteroids, std::vector<Shoot> &playerBullets);
void handleCollision(PlayerSteer& player, std::vector<Shoot>& enemyBullets);

#endif
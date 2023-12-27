#ifndef ASTEROID_ENEMY_H
#define ASTEROID_ENEMY_H

#include "EntityComponents.h"

#include "Munition.h"
#include "Player.hpp"


#include <vector>

struct Enemy{
  float value = 10;
  PhysicsComponent data;
  float shoot_counter = 0;
};

//update
void UpdateEnemy(Enemy& enemy, const Vector2& worldBound,
                 std::vector<Shoot> &shoots, float dt,
                 const PlayerState& player);

//paint
void PaintEnemy(Enemy& Enemy);

//factory
Enemy CreateEnemy(const Vector2 &worldBound);

#endif

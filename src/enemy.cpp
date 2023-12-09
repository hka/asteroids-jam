#include "enemy.h"

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void UpdateEnemy(Enemy& enemy, const Vector2& worldBound){
  UpdateMovement(enemy.position, enemy.movement, worldBound);
}

////////////////////////////////////////////////
///         Paint                           ///
///////////////////////////////////////////////
void PaintEnemy(Enemy& enemy){
  DrawCircle(enemy.position.x, enemy.position.y, enemy.radius, RED);
  DrawCircle(enemy.position.x+7, enemy.position.y+7, 5, DARKGRAY);

}

////////////////////////////////////////////////
///         Factory                         ///
///////////////////////////////////////////////
Enemy CreateEnemy(const Vector2& worldBound){
  Enemy enemy;

  enemy.radius = 20;

  enemy.position = getRandomPosOutsideBounds({0.f,0.f, worldBound.x, worldBound.y}, enemy.radius);

  const float maxAcceleration = 50.f;
  const float minAcceleration = 15.f;
  const float radiusPercentage = 1.f;

  enemy.movement.currentAcceleration = (maxAcceleration * radiusPercentage) + minAcceleration; //todo calculate init value

  enemy.movement.rotation = (float)GetRandomValue(0, 7);
  return enemy;
}

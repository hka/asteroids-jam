#include "enemy.h"

#include <limits>
#include <math.h>

#include "globals.h"
#include "raylib_operators.h"

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void UpdateEnemy(Enemy& enemy, const Vector2& worldBound,
                 std::vector<Shoot> &shoots, float dt,
                 const PlayerState& player)
{
  ApplyThrustDrag(enemy.data);
  UpdatePosition(enemy.data, worldBound, dt);

  enemy.shoot_counter += dt;
  if(enemy.shoot_counter > 2)
  {
    Vector2 aim_point = player.data.position + player.data.velocity*dt*60;
    Vector2 gun_direction = Vector2Normalize(aim_point - enemy.data.position);
    FireShoot(enemy.data, gun_direction,500, shoots);
    enemy.shoot_counter = 0;
  }

}

////////////////////////////////////////////////
///         Paint                           ///
///////////////////////////////////////////////
void PaintEnemy(Enemy& enemy){
  DrawCircle(enemy.data.position.x, enemy.data.position.y, enemy.data.radius, MAROON);
  DrawCircle(enemy.data.position.x+7, enemy.data.position.y+7, 5, DARKGRAY);

}

////////////////////////////////////////////////
///         Factory                         ///
///////////////////////////////////////////////
Enemy CreateEnemy(const Vector2& worldBound){
  Enemy enemy;

  float radius = 20;
  enemy.data.radius = radius;
  enemy.data.position = getRandomPosOutsideBounds({0.f,0.f, worldBound.x, worldBound.y}, radius);

  enemy.data.mass = M_PI*radius*radius;
  enemy.data.drag = 1.2f*radius/ASTEROID_MAX_RADIUS; //controlls terminal velocity should depend on radius maybe
  enemy.data.thrust = 100000; //todo calculate reasonable values
  std::uniform_real_distribution<> distrib(-1.f, 1.f);
  enemy.data.orientation = {(float)distrib(RNG),(float)distrib(RNG)};
  enemy.data.orientation = Vector2Normalize(enemy.data.orientation);

  const float maxAcceleration = 50.f;
  const float minAcceleration = 15.f;
  const float radiusPercentage = 1.f;

  return enemy;
}

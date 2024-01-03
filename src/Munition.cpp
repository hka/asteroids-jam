#include "Munition.h"

#include "raylib_operators.h"
#include "globals.h"

void RemoveOldShoots(std::vector<Shoot>& shoots, float t)
{
  std::vector<size_t> remove_ix;
  for(size_t ii = 0; ii < shoots.size(); ++ii)
  {
    if(shoots[ii].time_alive > t)
    {
      remove_ix.push_back(ii);
    }
  }
  //remove shoots by moving them to the end and shrinking
  for(size_t ii = 0; ii < remove_ix.size(); ++ii)
  {
    size_t ix = remove_ix[remove_ix.size() - 1 - ii];
    std::swap(shoots.back(), shoots[ix]);
    shoots.pop_back();
  }
}

void UpdateShoots(std::vector<Shoot>& shoots, float dt)
{
  Rectangle bound {0,0,(float)options.screenWidth,(float)options.screenHeight};
  Vector2 worldBound = {(float)options.screenWidth,(float)options.screenHeight};
  std::vector<size_t> remove_ix;
  for(size_t ii = 0; ii < shoots.size(); ++ii)
  {
    UpdatePosition(shoots[ii].data, worldBound, dt, true);
    shoots[ii].time_alive += dt;
    if(!CheckCollisionPointRec(shoots[ii].data.position, bound))
    {
      //bullet left screen
      remove_ix.push_back(ii);
    }
  }
  //remove shoots by moving them to the end and shrinking
  for(size_t ii = 0; ii < remove_ix.size(); ++ii)
  {
    size_t ix = remove_ix[remove_ix.size() - 1 - ii];
    std::swap(shoots.back(), shoots[ix]);
    shoots.pop_back();
  }
}

void DrawShoots(const std::vector<Shoot>& shoots)
{
  for(const Shoot& s : shoots)
  {
    //DrawCircleV(s.data.position, s.data.radius, DARKGRAY);
    DrawCircleGradient(s.data.position.x, s.data.position.y, s.data.radius, WHITE, BLUE);
  }
}

void FireShoot(const PhysicsComponent& player, const Vector2& direction, float muzzle_velocity, std::vector<Shoot> &shoots)
{
  Shoot bullet;
  bullet.data.position = player.position + 30 * direction;
  bullet.data.orientation = direction;
  bullet.data.radius = 4;
  bullet.data.mass = 1;
  bullet.damage = 1;
  Vector2 norm_dir = Vector2Normalize(direction);
  bullet.data.velocity = player.velocity + norm_dir*muzzle_velocity;
  shoots.push_back(bullet);
}

void FireShootgun(const PhysicsComponent& playerdata, int& storedAsteroids, const Vector2& direction, float muzzle_velocity, std::vector<Shoot> &shoots)
{
  if(storedAsteroids == 0)
  {
    return;
  }
  for(int ii = 0; ii < storedAsteroids; ++ii)
  {
    Shoot bullet;
    bullet.data.position = playerdata.position + 30 * direction;
    Vector2 ort_dir = {direction.y,-direction.x};
    bullet.data.position += ort_dir*ii*20;
    bullet.data.orientation = direction;
    bullet.data.radius = 10;
    bullet.data.mass = 1;
    Vector2 norm_dir = Vector2Normalize(direction);
    bullet.data.velocity = playerdata.velocity + norm_dir*muzzle_velocity;
    shoots.push_back(bullet);
  }
  storedAsteroids = 0;
}

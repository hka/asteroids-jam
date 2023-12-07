#include "Munition.h"

#include "raylib_operators.h"
#include "globals.h"

void UpdateShoots(std::vector<Shoot>& shoots)
{
  Rectangle bound {0,0,(float)options.screenWidth,(float)options.screenHeight};
  std::vector<size_t> remove_ix;
  for(size_t ii = 0; ii < shoots.size(); ++ii)
  {
    shoots[ii].position += shoots[ii].direction*shoots[ii].speed*GetFrameTime();
    if(!CheckCollisionPointRec(shoots[ii].position, bound))
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
    DrawCircleV(s.position, s.radius, DARKGRAY);
  }
}

void FireShoot(PlayerSteer& player)
{
  Shoot bullet;
  bullet.position = player.position + 30*player.gunDirection;
  bullet.direction = player.gunDirection;
  bullet.radius = 4;
  float vel_component = Vector2Length(proj(player.velocity,player.gunDirection));
  float sign = dot(player.gunDirection, player.velocity) < 0 ? -1 : 1;
  bullet.speed = vel_component*sign + 2*player.maxAcceleration;
  SHOOTS.push_back(bullet);
}
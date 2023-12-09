#include "enemy.h"

#include <limits>
#include <math.h>

#include "globals.h"

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void UpdateEnemy(Enemy& enemy, const Vector2& worldBound,
                 const std::vector<Asteroid>& asteroids)
{
  //check if any asteroid close
  //this code doesnt handle wrapping!
  const size_t bin_count = 60;
  std::vector<float> dir_dist_hist(bin_count);
  std::vector<float> dir_dist_hist_count(bin_count);
  float bin_width = M_PI/72; //5 degree per bin -pi:-0 in 0:35, 0:pi in 36:71
  float closest_dist = std::numeric_limits<float>::max();
  for(const Asteroid& a : asteroids)
  {
    float d = Vector2DistanceSqr(a.position,enemy.position);
    closest_dist = std::min(d, closest_dist);
    float angle = Vector2LineAngle(enemy.position, a.position); //slow

    angle += M_PI; //easier insert, all positive
    angle = angle < 0 ? 2*M_PI : angle; //clamp
    size_t angle_ix = (size_t)(angle/bin_width+0.5);
    if(angle_ix < dir_dist_hist.size())
    {
      dir_dist_hist[angle_ix] += d;
      dir_dist_hist_count[angle_ix] += d;
    }
  }

  float best_new_dir = 0;
  float best_new_dir_score = 0;
  if(closest_dist < 4*enemy.radius*enemy.radius)
  {
    //check if any 5 deg sector is empty
    std::vector<float> empty_dirs;
    for(size_t ix0 = 0; ix0 < bin_count; ++ix0)
    {
      if(dir_dist_hist_count[ix0] == 0)
      {
        float dir = ix0*bin_width + bin_width/2 - M_PI;
        empty_dirs.push_back(dir);
      }
    }
    if(empty_dirs.size() > 0)
    {
      //pick one at random
      std::uniform_int_distribution<> distrib(0, empty_dirs.size() -1);
      size_t dir_ix = (size_t)distrib(RNG);
      best_new_dir = empty_dirs[dir_ix];
      best_new_dir_score = 100; //anything none zero
    }
    else
    {
      for(size_t ix0 = 0; ix0 < bin_count; ++ix0)
      {
        size_t ix1 = (ix0+1)%bin_count;
        float d0 = dir_dist_hist_count[ix0];
        float n0 = dir_dist_hist[ix0];
        float d1 = dir_dist_hist_count[ix1];
        float n1 = dir_dist_hist[ix1];

        //should not get here if any of d0, d1 is zero, but
        //better check anyway
        float dir_score = d0+d1 > 0 ? (n0+n1)/(d0+d1) : 0;
        if(dir_score > best_new_dir_score)
        {
          best_new_dir_score = dir_score;
          float ix = (ix0*n0 + ix1*n1)/(d0+d1);
          float dir = ix*bin_width + bin_width/2 - M_PI;
          best_new_dir = 0;
        }
      }
    }
    //update movement vector
    enemy.movement.rotation = best_new_dir;
    //printf("enemy dir: %f, update prop: %f\n", enemy.movement.rotation, best_new_dir);
  }

  UpdateMovement(enemy.position, enemy.movement, worldBound);
}

////////////////////////////////////////////////
///         Paint                           ///
///////////////////////////////////////////////
void PaintEnemy(Enemy& enemy){
  DrawCircle(enemy.position.x, enemy.position.y, enemy.radius, MAROON);
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

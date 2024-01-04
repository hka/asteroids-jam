#include "Asteroid.h"

#include "globals.h"

#include <raymath.h>
#include "raylib_operators.h"

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void UpdateAsteroid(Asteroid& asteroid, const Vector2& worldBound, std::vector<Shoot> &shoots, float dt)
{
  ApplyThrustDrag(asteroid.data);
  UpdatePosition(asteroid.data, worldBound, dt);
  if(asteroid.type == 3)
  {
    asteroid.data.orientation = Vector2Rotate(asteroid.data.orientation, 10*(M_PI/180.f)*dt);

    asteroid.shoot_counter += dt;
    if(asteroid.shoot_counter > 2)
    {
      std::vector<Vector2> guns;
      Vector2 dir = asteroid.data.orientation;
      Vector2 orth_dir = {dir.y, -dir.x};
      Vector2 pos = asteroid.data.position;
      guns.push_back(pos + asteroid.data.radius*dir);
      guns.push_back(pos - asteroid.data.radius*dir);
      guns.push_back(pos + asteroid.data.radius*orth_dir);
      guns.push_back(pos - asteroid.data.radius*orth_dir);

      for(size_t ii = 0; ii < guns.size(); ++ii)
      {
        float x = guns[ii].x;
        float y = guns[ii].y;
        Vector2 gun_direction = Vector2Normalize({x - pos.x, y - pos.y});
        FireShoot(asteroid.data, gun_direction,500, shoots);
      }
      asteroid.shoot_counter = 0;
    }
  }
}

////////////////////////////////////////////////
///         Helper                         ///
///////////////////////////////////////////////
void OnAsteroidSplit(std::vector<Asteroid> &asteroids, const int type, const Vector2& pos)
{
  if (type == 1)
  {
    // nothing, just destroyed
  }
  else if (type == 2)
  {
    for (size_t ii = 0; ii < 5; ++ii)
    {
      asteroids.push_back(CreateAsteroid(pos, 1));
    }
  }
  else if (type == 3)
  {
    int count = GetRandomValue(2, 3);
    for (int ii = 0; ii < count; ++ii)
    {
      asteroids.push_back(CreateAsteroid(pos, 2));
    }
  }
}

////////////////////////////////////////////////
///         Paint                           ///
///////////////////////////////////////////////
void PaintAsteroid(Asteroid& asteroid){
  //DrawCircle(asteroid.data.position.x, asteroid.data.position.y, asteroid.data.radius, BROWN);

  float r = asteroid.data.radius;
  float x = asteroid.data.position.x;
  float y = asteroid.data.position.y;

  Texture2D te = TEXTURES[asteroid.art_ix];
  // Source rectangle (part of the texture to use for drawing)
  Rectangle sourceRec = { 0.0f, 0.0f, (float)te.width, (float)te.height };

  // Destination rectangle (screen rectangle where drawing part of texture)
  Rectangle destRec = { x, y, 2*r, 2*r };
  // Origin of the texture (rotation/scale point), it's relative to destination rectangle size
  Vector2 origin = { r,r };

  int rotation = atan2(asteroid.data.orientation.y,asteroid.data.orientation.x)*180/M_PI + 90;

  Color c = asteroid.shouldDamageBlink ? RED : WHITE;
  DrawTexturePro(te, sourceRec, destRec, origin, (float)rotation, c);
  if(asteroid.type == 3)
  {
    Texture2D te = TEXTURES[1];
    std::vector<Vector2> guns;
    Vector2 dir = asteroid.data.orientation;
    Vector2 orth_dir = {dir.y, -dir.x};
    guns.push_back(asteroid.data.position + asteroid.data.radius*dir);
    guns.push_back(asteroid.data.position - asteroid.data.radius*dir);
    guns.push_back(asteroid.data.position + asteroid.data.radius*orth_dir);
    guns.push_back(asteroid.data.position - asteroid.data.radius*orth_dir);
    for(size_t ii = 0; ii < guns.size(); ++ii)
    {
      Rectangle sourceRec = { 0.0f, 0.0f, (float)te.width, (float)te.height };
      float r = asteroid.data.radius/2;
      float x = guns[ii].x;
      float y = guns[ii].y;
      Rectangle destRec = { x, y, 2*r, 2*r };
      Vector2 origin = { r,r };
      float dx = x - asteroid.data.position.x;
      float dy = y - asteroid.data.position.y;
      float rot = atan2(dy,dx)*180/M_PI + 90.f;
      DrawTexturePro(te, sourceRec, destRec, origin, rot, WHITE);
    }
  }
}

////////////////////////////////////////////////
///         Factory                         ///
///////////////////////////////////////////////
Asteroid CreateAsteroidR(const Vector2& worldBound)
{
  int type = GetRandomValue(1, 3);
  return CreateAsteroidR(worldBound, type);
}
Asteroid CreateAsteroidR(const Vector2& worldBound, int type){
  Asteroid asteroid;
  asteroid.type = type;
  float radius = asteroid.type*options.screenWidth*ASTEROID_SCALE;

  switch(asteroid.type)
  {
  case 1:
    asteroid.hp = 1;
    break;
  case 2:
    asteroid.hp = 3;
    break;
  case 3:
    asteroid.hp = 5;
  }

  asteroid.data.radius = radius;
  asteroid.data.position = getRandomPosOutsideBounds({0.f,0.f, worldBound.x, worldBound.y}, radius);

  asteroid.data.mass = M_PI*radius*radius;
  asteroid.data.drag = 1.2f*radius/ASTEROID_MAX_RADIUS; //controlls terminal velocity should depend on radius maybe
  asteroid.data.thrust = 5000; //todo calculate reasonable values
  std::uniform_real_distribution<> distrib(-1.f, 1.f);
  asteroid.data.orientation = {(float)distrib(RNG),(float)distrib(RNG)};
  asteroid.data.orientation = Vector2Normalize(asteroid.data.orientation);

  //initial velocity
  asteroid.data.velocity = asteroid.data.orientation*50;

  std::uniform_int_distribution<> distrib_art(2, 4);
  asteroid.art_ix = distrib_art(RNG);
  
  asteroid.state = ALIVE;
  asteroid.shouldDamageBlink = false;

  return asteroid;
}

Asteroid CreateAsteroid(const Vector2 &pos, int type){
  Asteroid asteroid;

  float radius = type*options.screenWidth*ASTEROID_SCALE;

  switch(type)
  {
  case 1:
    asteroid.hp = 1;
    break;
  case 2:
    asteroid.hp = 3;
    break;
  case 3:
    asteroid.hp = 5;
  }

  asteroid.type = type;
  asteroid.data.radius = radius;
  asteroid.data.position = pos;

  asteroid.data.mass = M_PI * radius * radius;
  asteroid.data.drag = 1.2f * radius / ASTEROID_MAX_RADIUS; // controlls terminal velocity should depend on radius maybe
  asteroid.data.thrust = 5000;                             // todo calculate reasonable values
  std::uniform_real_distribution<> distrib(-1.f, 1.f);
  asteroid.data.orientation = {(float)distrib(RNG), (float)distrib(RNG)};
  asteroid.data.orientation = Vector2Normalize(asteroid.data.orientation);

  std::uniform_int_distribution<> distrib_art(2, 4);
  asteroid.art_ix = distrib_art(RNG);

  asteroid.state = ALIVE;
  asteroid.shouldDamageBlink = false;
  return asteroid;
}

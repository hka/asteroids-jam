#include "Asteroid.h"

#include "globals.h"

#include <raymath.h>

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void UpdateAsteroid(Asteroid& asteroid, const Vector2& worldBound, float dt)
{
  ApplyThrustDrag(asteroid.data);
  UpdatePosition(asteroid.data, worldBound, dt);
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

  int rotation = 0;


  DrawTexturePro(te, sourceRec, destRec, origin, (float)rotation, WHITE);
}

////////////////////////////////////////////////
///         Factory                         ///
///////////////////////////////////////////////
Asteroid CreateAsteroid(const Vector2& worldBound){
  Asteroid asteroid;
  float radius = GetRandomValue(ASTEROID_MIN_RADIUS, ASTEROID_MAX_RADIUS);
  asteroid.data.radius = radius;
  asteroid.data.position = getRandomPosOutsideBounds({0.f,0.f, worldBound.x, worldBound.y}, radius);

  asteroid.data.mass = M_PI*radius*radius;
  asteroid.data.drag = 1.2f*radius/ASTEROID_MAX_RADIUS; //controlls terminal velocity should depend on radius maybe
  asteroid.data.thrust = 5000; //todo calculate reasonable values
  std::uniform_real_distribution<> distrib(-1.f, 1.f);
  asteroid.data.orientation = {(float)distrib(RNG),(float)distrib(RNG)};
  asteroid.data.orientation = Vector2Normalize(asteroid.data.orientation);

  std::uniform_int_distribution<> distrib_art(2, 4);
  asteroid.art_ix = distrib_art(RNG);
  
  return asteroid;
}

Asteroid CreateAsteroid(const Vector2 &pos, float radius){
  Asteroid asteroid;

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

  return asteroid;
}

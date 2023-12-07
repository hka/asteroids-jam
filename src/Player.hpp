#ifndef _Player
#define _Player

#include "raylib.h"
#include "raymath.h"

inline void rotateTriangle(Vector2 (&v)[3], const float angle);
inline void updateVelocity(const float rotation, Vector2& velocity, const float acceleration);
inline void updatePosition(Vector2 (&v)[3], Vector2& velocity, const float dt);

struct PlayerSteer
{
    Vector2 position;
    Vector2 vertices[3];
    float rotation;
    float rotationSpeed;

    Vector2 velocity;
    float currentAcceleration;
    float maxAcceleration;
    float accelerationDecrease;
}typedef PlayerSteer;


inline PlayerSteer createPlayer(Vector2 startPos){
    PlayerSteer player;
    player.position = startPos;
    //todo define lengths of triangle
    player.vertices[0] = player.position;
    player.vertices[1] = {player.position.x - 30.f, player.position.y - 10.f};
    player.vertices[2] = {player.position.x - 30.f, player.position.y + 10.f};

    player.maxAcceleration = 250.f;
    player.velocity = {0.f,0.f};
    player.currentAcceleration = 0.f;
    player.accelerationDecrease = 10.f;
    player.rotation = 0.f;
    player.rotationSpeed = 5.f;

    return player;
}

inline void rotateShip(PlayerSteer& player){
  if(IsKeyDown(KEY_LEFT)){
    float angle = player.rotationSpeed * GetFrameTime();
    player.rotation -= angle;
    rotateTriangle(player.vertices, -angle);
  }else if(IsKeyDown(KEY_RIGHT)){
    float angle = player.rotationSpeed * GetFrameTime();
    player.rotation += angle;
    rotateTriangle(player.vertices, angle);
  }
}

inline void accelerate(PlayerSteer& player){
  if(IsKeyDown(KEY_UP)){
    player.currentAcceleration = player.maxAcceleration;
  }else{
    float newAccleration = player.currentAcceleration - player.accelerationDecrease;
    player.currentAcceleration = newAccleration > 0.f ? newAccleration : 0.f;
  }
}

inline void rotateTriangle(Vector2 (&v)[3], const float angle){
  Vector2 center = {(v[0].x + v[1].x + v[2].x) / 3.f, (v[0].y + v[1].y + v[2].y) / 3};
  v[0] = Vector2Add(Vector2Rotate(Vector2Subtract(v[0], center), angle), center);
  v[1] = Vector2Add(Vector2Rotate(Vector2Subtract(v[1], center), angle), center);
  v[2] = Vector2Add(Vector2Rotate(Vector2Subtract(v[2], center), angle), center);
}

inline void updateVelocity(const float rotation, Vector2& velocity, const float acceleration){
  Vector2 newVelocity = Vector2Normalize({cosf(rotation), sinf(rotation)});
  newVelocity.x *= acceleration;
  newVelocity.y *= acceleration;
  velocity = newVelocity;
}

inline void updatePosition(Vector2 (&v)[3], Vector2& velocity, const float dt){
  for(int i = 0; i < 3; ++i){
    Vector2 newPos = Vector2Add(v[i],  Vector2Scale(velocity,dt));
    v[i].x = newPos.x;
    v[i].y = newPos.y;
  }
}

#endif

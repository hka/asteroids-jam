#ifndef _Entities
#define _Entities

#include "raylib.h"

struct PlayerSteer
{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    Vector2 vertices[3];
    float rotation;
}typedef PlayerOne;

#endif
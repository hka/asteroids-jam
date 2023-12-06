#ifndef _Player
#define _Player

#include "raylib.h"

struct PlayerSteer
{
    Vector2 position;
    Vector2 vertices[3];
    float rotation;

    Vector2 velocity;
    float currentAcceleration;
    float maxAcceleration;
    float accelerationDecrease;
}typedef PlayerOne;

#endif

#ifndef ASTEROID_COLOR_BELENDER_H_
#define ASTEROID_COLOR_BELENDER_H_

#include "raylib.h"

inline Color BlendColors(Color c1, Color c2)
{
  Color blended;

  blended.r = (c1.r < 128) ? (2 * c1.r * c2.r / 255) : (255 - 2 * (255 - c1.r) * (255 - c2.r) / 255);
  blended.g = (c1.g < 128) ? (2 * c1.g * c2.g / 255) : (255 - 2 * (255 - c1.g) * (255 - c2.g) / 255);
  blended.b = (c1.b < 128) ? (2 * c1.b * c2.b / 255) : (255 - 2 * (255 - c1.b) * (255 - c2.b) / 255);

  return blended;
}

#endif
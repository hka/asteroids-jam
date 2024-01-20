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

inline Color BlendColors(Color c1, Color c2, Color c3)
{
  Color blended;

  // Intermediate blend of c1 and c2
  Color intermediate;
  intermediate.r = (c1.r < 128) ? (2 * c1.r * c2.r / 255) : (255 - 2 * (255 - c1.r) * (255 - c2.r) / 255);
  intermediate.g = (c1.g < 128) ? (2 * c1.g * c2.g / 255) : (255 - 2 * (255 - c1.g) * (255 - c2.g) / 255);
  intermediate.b = (c1.b < 128) ? (2 * c1.b * c2.b / 255) : (255 - 2 * (255 - c1.b) * (255 - c2.b) / 255);

  // Final blend with c3
  blended.r = (intermediate.r < 128) ? (2 * intermediate.r * c3.r / 255) : (255 - 2 * (255 - intermediate.r) * (255 - c3.r) / 255);
  blended.g = (intermediate.g < 128) ? (2 * intermediate.g * c3.g / 255) : (255 - 2 * (255 - intermediate.g) * (255 - c3.g) / 255);
  blended.b = (intermediate.b < 128) ? (2 * intermediate.b * c3.b / 255) : (255 - 2 * (255 - intermediate.b) * (255 - c3.b) / 255);

  return blended;
}

Color BlendColorsSimple(Color c1, Color c2)
{
  Color blended;
  float alphaBlend = c2.a / 255.0f;

  blended.r = c1.r * (1.0f - alphaBlend) + c2.r * alphaBlend;
  blended.g = c1.g * (1.0f - alphaBlend) + c2.g * alphaBlend;
  blended.b = c1.b * (1.0f - alphaBlend) + c2.b * alphaBlend;
  blended.a = 255; // Set to fully opaque

  return blended;
}

Color BlendColorsNormalized(Color laser, Color noise)
{
  Color blended;

  blended.r = laser.r * (noise.r / 128.0f);
  blended.g = laser.g * (noise.g / 128.0f);
  blended.b = laser.b * (noise.b / 128.0f);
  blended.a = 255; // Set to fully opaque

  return blended;
}

Color BlendColorsWithAlpha(Color laser, Color noise)
{
  Color blended;

  float alphaBlend = noise.a / 255.0f;
  blended.r = laser.r * (1.0f - alphaBlend) + laser.r * (noise.r / 128.0f) * alphaBlend;
  blended.g = laser.g * (1.0f - alphaBlend) + laser.g * (noise.g / 128.0f) * alphaBlend;
  blended.b = laser.b * (1.0f - alphaBlend) + laser.b * (noise.b / 128.0f) * alphaBlend;
  blended.a = 255; // Set to fully opaque

  return blended;
}

Color BlendColorsWithAlpha(Color c1, Color c2, float blendFactor)
{
  Color blended;

  blended.r = c1.r * (1.0f - blendFactor) + c2.r * blendFactor;
  blended.g = c1.g * (1.0f - blendFactor) + c2.g * blendFactor;
  blended.b = c1.b * (1.0f - blendFactor) + c2.b * blendFactor;
  blended.a = 255; // Set to fully opaque

  return blended;
}


Color BlendColorsWithAlpha(Color laser, Color noise, Color opposite, float blendFactor)
{
  Color blended;

  float alphaBlend = noise.a / 255.0f;

  // Blending laser and noise based on noise alpha
  Color intermediate;
  intermediate.r = laser.r * (1.0f - alphaBlend) + laser.r * (noise.r / 128.0f) * alphaBlend;
  intermediate.g = laser.g * (1.0f - alphaBlend) + laser.g * (noise.g / 128.0f) * alphaBlend;
  intermediate.b = laser.b * (1.0f - alphaBlend) + laser.b * (noise.b / 128.0f) * alphaBlend;

  // Blending the result with the opposite color based on blendFactor
  blended.r = intermediate.r * (1.0f - blendFactor) + opposite.r * blendFactor;
  blended.g = intermediate.g * (1.0f - blendFactor) + opposite.g * blendFactor;
  blended.b = intermediate.b * (1.0f - blendFactor) + opposite.b * blendFactor;
  blended.a = 255; // Set to fully opaque

  return blended;
}

#endif
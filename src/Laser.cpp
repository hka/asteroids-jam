#include "Laser.h"

#include "raymath.h"
#include "ResourceManager.h"
#include "ColorBlender.h"

Laser createLaser(){
  Laser laser;
  laser.isOngoing = false;
  laser.maxLength = LASER_MAX_LENGTH;
  laser.growRate = laser.maxLength * 4.f;
  laser.width = LASER_HEIGHT; //todo set to screen width.
  laser.duration = 4.0f;
  laser.graphics = createLaserGraphics();
  return laser;
}

LaserGraphics createLaserGraphics(){
  LaserGraphics graphics;

  int width = LASER_MAX_LENGTH;
  int height = LASER_HEIGHT;
  graphics.laserOriginalPixels.resize(width * height);

  int steps = (height / 2);
  Color c = {255, 0, 0, 255};
  unsigned char rTarget = 50;
  unsigned char aTarget = 50;

  unsigned char rStep = (255 - rTarget) / steps;
  unsigned char aStep = (255 - aTarget) / steps;

  //fade out red color
  for (int y = 0; y < (height / 2) - 1; ++y)
  {
    int y1 = (height / 2) - (y + 1);
    int y2 = (height / 2) + (y + 1);
    for (int x = 0; x < width; ++x)
    {
      graphics.laserOriginalPixels[y1 * width + x] = c;
      graphics.laserOriginalPixels[y2 * width + x] = c;
    }
    c.r -= rStep;
    c.a -= aStep;
  }

  //set white lines in middle
  int y = height / 2.f;
  c = {255, 255, 255, 255};
  for (int x = 0; x < width - 2; ++x)
  {
    graphics.laserOriginalPixels[y * width + x] = c;
  }

  //todo generate own perlin noise instead.
  Image noiseMap = GenImagePerlinNoise(width, height, 0, 0, 10.f);
  Color* noisePixels = (Color*) noiseMap.data;
  for(int i = 0; i < (width * height); ++i)
  {
    graphics.noiseMap.emplace_back(noisePixels[i]);
  }
  UnloadImage(noiseMap);

  Image distortionMap = GenImageCellular(width, height, 30);
  Color *distortionPixels = (Color *)distortionMap.data;
  for (int i = 0; i < (width * height); ++i)
  {
    graphics.distortionMap.emplace_back(distortionPixels[i]);
  }
  UnloadImage(distortionMap);

  return graphics;
}

void CreateLaserTexture(){
  Image finalLaser = GenImageColor(LASER_MAX_LENGTH, LASER_HEIGHT, BLACK);
  ImageMipmaps(&finalLaser);
  LoadTexture(finalLaser, LASER_FINAL);
  LoadImage(IMAGE_LASER_FINAL, finalLaser);
}

void OnStart(Laser &laser, const Vector2 &direction, const Vector2 &origin){
  laser.isOngoing = true;
  laser.direction = direction;
  laser.start = origin;
  laser.length = 0.f;
  laser.timer.start();
  laser.noiseOffset = 0.f;
}

void Update(Laser &laser, const Vector2 &direction, const Vector2 &origin)
{

  if(laser.timer.getElapsed() >= laser.duration){
    laser.isOngoing = false;
    return;
  }

  laser.start = {
      origin.x + (direction.x * 15.f),
      origin.y + (direction.y * 15.f),
  }; 
  laser.direction = direction;

  laser.ray.direction = {laser.direction.x, laser.direction.y, 0.f};
  laser.ray.position = {laser.start.x, laser.start.y, 0.f};

  laser.end = {
    laser.start.x + (laser.direction.x * laser.length),
    laser.start.y + (laser.direction.y * laser.length)
  };

  if(laser.length < laser.maxLength)
  {
    laser.length += (laser.growRate * GetFrameTime());
  }

  NoiseLaserWeb(laser.noiseOffset, laser.graphics);
  DistortLaserWeb(laser.graphics);
}

void Clear(Laser &laser)
{
  laser.isOngoing = false;
}

void DrawLaser(Laser &laser){
  
  float rotation = atan2f(laser.direction.y, laser.direction.x) * RAD2DEG;
  Rectangle rec = {laser.start.x, laser.start.y, laser.length, laser.width};
  Rectangle src = {0.f, 0.f, laser.maxLength, laser.width};
  Vector2 origin {0.f, laser.width/ 2.f};
  
  DrawTexturePro(getTexture(LASER_FINAL), src, rec, origin, rotation, WHITE);
}

void NoiseLaserWeb(float &noiseOffset, LaserGraphics &graphics)
{

  float noiseSpeed = -5.f;
  noiseOffset += noiseSpeed;

  int width = LASER_MAX_LENGTH;
  int height = LASER_HEIGHT;

  Image finalLaserImage = GetImage(IMAGE_LASER_FINAL);
  for(int y = 0; y < height; ++y){
    for(int x = 0; x < width; ++x){
      int index = y * width + x;
      Color laserColor = graphics.laserOriginalPixels[index];
      int noiseX = ((x + (int)noiseOffset) % width + width) % width;
      int noiseY = ((y + (int)noiseOffset) % height + height) % height;
      Color noiseColor = graphics.noiseMap[noiseY * width + noiseX];

      if (noiseX > width - 5)
      {
        int startNoiseX = (noiseX + 5) % width;
        Color startNoiseColor = graphics.noiseMap[noiseY * width + startNoiseX];
        float blendFactorX = (width - noiseX) / 5.0f;
        noiseColor = BlendColorsWithAlpha(noiseColor, startNoiseColor, blendFactorX);
      }

      if (noiseY > height - 5)
      {
        int startNoiseY = (noiseY + 5) % height;
        Color startNoiseColor = graphics.noiseMap[startNoiseY * width + noiseX];
        float blendFactorY = (height - noiseY) / 5.0f;
        noiseColor = BlendColorsWithAlpha(noiseColor, startNoiseColor, blendFactorY);
      }

      Color finalColor = BlendColorsWithAlpha(laserColor, noiseColor);
      finalColor.a = 255;
      ImageDrawPixel(&finalLaserImage, x, y, finalColor);
    }
  }

  UpdateTexture(LASER_FINAL, finalLaserImage.data);

}

void DistortLaserWeb(LaserGraphics& graphics){
  int height = (int)LASER_HEIGHT;
  int width = (int)LASER_MAX_LENGTH;

  static float distortionOffsetX = 0.0f;
  static float distortionOffsetY = 0.0f;

  float distortionSpeed = -1.f;
  float maxDistortion = 15.f;

  distortionOffsetX += distortionSpeed;
  if (distortionOffsetX >= width)
    distortionOffsetX -= width;

  distortionOffsetY += distortionSpeed;
  if (distortionOffsetY >= height)
    distortionOffsetY -= height;

  Image laserImage = GetImage(IMAGE_LASER_FINAL);

  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      int distortionX = (x + (int)distortionOffsetX) % width;
      int distortionY = (y + (int)distortionOffsetY) % height;
      Color distortionColor = graphics.distortionMap[distortionY * width + distortionX];
      float distortionAmount = (distortionColor.r / 255.0f - 0.5f) * maxDistortion;
      int srcX = x + distortionAmount;
      int srcY = y + distortionAmount;
      srcX = Clamp(srcX, 0, width - 1);
      srcY = Clamp(srcY, 0, height - 1);

      Color finalColor = graphics.laserOriginalPixels[srcY * width + srcX];
      ImageDrawPixel(&laserImage, x, y, finalColor);
    }
  }
  UpdateTexture(LASER_FINAL, laserImage.data);
}

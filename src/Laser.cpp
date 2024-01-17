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
  laser.duration = 3.0f;
  return laser;
}

void CreateLaserTexture(){
  int height = LASER_HEIGHT;
  int maxLength = LASER_MAX_LENGTH;
  Color background = {255,0,0,255};
  Image imageLaser = GenImageColor(maxLength, height, background);
  
  int steps = (height / 2);
  Color c = {255, 0, 0, 255};
  unsigned char rTarget = 50;
  unsigned char aTarget = 50;

  unsigned char rStep = (255 - rTarget) / steps;
  unsigned char aStep = (255 - aTarget) / steps;

  for(int y = 0; y < (height/2); ++y){
    int y1 = (height / 2) - (y + 1);
    int y2 = (height / 2) + (y + 1);
    for(int x = 0; x < maxLength; ++x){
      ImageDrawPixel(&imageLaser, x, y1, c);
      ImageDrawPixel(&imageLaser, x, y2, c);
    }
    c.r -= rStep;
    c.a -= aStep;
  }

  int y = height/2.f;
  c = {255,255,255,255};
  for(int x = 0; x < maxLength - 2; ++x){
    ImageDrawPixel(&imageLaser, x, y, c);
  }
  ImageAlphaPremultiply(&imageLaser);
  ImageMipmaps(&imageLaser);
  LoadImage(IMAGE_LASER, imageLaser);

  Image blurred = GetImage(IMAGE_LASER);
  ImageBlurGaussian(&blurred, 1);
  ImageMipmaps(&blurred);
  LoadTexture(blurred, LASER_BLURRED);
  LoadImage(IMAGE_LASER_BLURRED, blurred);

  Image laserNoise = GenImagePerlinNoise(maxLength, height, 0, 0, 5.f);
  LoadImage(IMAGE_LASER_NOISE, laserNoise);

  Image laserDistortion = GenImageCellular(maxLength, height,1);
  LoadImage(IMAGE_LASER_DISTORTION, laserDistortion);

  Image finalLaser = GenImageColor(maxLength, height, WHITE);
  ImageMipmaps(&finalLaser);
  LoadTexture(finalLaser, LASER_FINAL);
  LoadImage(IMAGE_LASER_FINAL, finalLaser);

  NoiseLaser();
  DistortLaser();
}

void OnStart(Laser &laser, const Vector2 &direction, const Vector2 &origin){
  laser.isOngoing = true;
  laser.direction = direction;
  laser.start = origin;
  laser.length = 0.f;
  laser.timer.start();
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

  NoiseLaser();
  DistortLaser();
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
  DrawTexturePro(getTexture(LASER_BLURRED), src, rec, origin, rotation, WHITE);
  DrawTexturePro(getTexture(LASER_FINAL), src, rec, origin, rotation, WHITE);
}

static float noiseOffsetX = 0.f;

void NoiseLaser(){

  int width = LASER_MAX_LENGTH;
  int height = LASER_HEIGHT;

  Color* laserPixels = (Color*) GetImage(IMAGE_LASER).data;
  Color *noisePixels = (Color*) GetImage(IMAGE_LASER_NOISE).data;
  
  float noiseSpeed = -25.f;
  noiseOffsetX += noiseSpeed;
  Color* finalLaserPixels = (Color*) GetImage(IMAGE_LASER_FINAL).data;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      Color laserColor = laserPixels[y * width + x];

      int noiseX = ((x + (int)noiseOffsetX) % width + width) % width;
      Color noiseColor = noisePixels[y * width + noiseX];

      Color finalColor = BlendColors(laserColor, noiseColor);
      finalLaserPixels[y * width + x] = finalColor;
    }
  }
  UpdateTexture(LASER_FINAL, finalLaserPixels);
  GetImage(IMAGE_LASER_FINAL).data = finalLaserPixels;
}

void DistortLaser(){
  int height = (int)LASER_HEIGHT;
  int width = (int)LASER_MAX_LENGTH;

  static float distortionOffsetX = 0.0f;
  static float distortionOffsetY = 0.0f;

  float distortionSpeed = 1.f;
  float maxDistortion = 20.f;

  distortionOffsetX += distortionSpeed;
  if (distortionOffsetX >= width)
    distortionOffsetX -= width;

  distortionOffsetY += distortionSpeed;
  if (distortionOffsetY >= height)
    distortionOffsetY -= height;


  Color* distortionMap = (Color*)GetImage(IMAGE_LASER_NOISE).data;
  Color *laserPixels = (Color *)GetImage(IMAGE_LASER_FINAL).data;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      int index = y * width + x;
      int distortionX = (x + (int)distortionOffsetX) % width;
      int distortionY = (y + (int)distortionOffsetY) % height;
      Color distortionColor = distortionMap[distortionY * width + distortionX];
      float distortionAmount = (distortionColor.r / 255.0f - 0.5f) * maxDistortion;

      int srcX = x + distortionAmount;
      int srcY = y + distortionAmount;
      srcX = Clamp(srcX, 0, width - 1);
      srcY = Clamp(srcY, 0, height - 1);
      Color finalColor = laserPixels[srcY * width + srcX];
      laserPixels[index] = finalColor;
    }
  }
  UpdateTexture(LASER_FINAL, laserPixels);
}

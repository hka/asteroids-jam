#ifndef ASTEROID_RESOURCE_MANAGER_H_
#define ASTEROID_RESOURCE_MANAGER_H_

#include <vector>

#include "raylib.h"

constexpr const int NUMBER_OF_IMAGES = 3;
enum ImageID{
  IMAGE_LASER,
  IMAGE_LASER_NOISE,
  IMAGE_LASER_DISTORTION,
  IMAGE_LASER_BLURRED,
  IMAGE_LASER_FINAL
};

constexpr const int NUMBER_OF_TEXTURES = 2;
enum TextureID{
  LASER_FINAL = 0,
  LASER_BLURRED
};

constexpr const int NUMBER_OF_RENDER_TEXTURES = 1;
enum RenderTextureID{
  RENDER_LASER = 0
};

struct Resources{
  std::vector<Texture2D> textures;
  std::vector<RenderTexture2D> renderTextures;
  std::vector<Image> images;
  Resources():textures(NUMBER_OF_TEXTURES), renderTextures(NUMBER_OF_RENDER_TEXTURES), images(NUMBER_OF_IMAGES){}
};

inline Resources RESOURCES;

void LoadTexture(const Image &image, TextureID id);
void UpdateTexture(TextureID id, const void* pixels);
const Texture2D& getTexture(TextureID id);

void LoadRenderTexture(RenderTextureID id, int width, int height);
RenderTexture2D& GetRenderTexture(RenderTextureID id);

void LoadImage(ImageID id, const Image& image);
Image& GetImage(ImageID id);

void Unload();

#endif
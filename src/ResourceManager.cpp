#include "ResourceManager.h"
#include "globals.h"

void LoadTexture(const Image &image, TextureID id){
  RESOURCES.textures[id] = LoadTextureFromImage(image);
}

void UpdateTexture(TextureID id, const void *pixels){
  UpdateTexture(RESOURCES.textures[id], pixels);
}

const Texture2D &getTexture(TextureID id){
  return RESOURCES.textures[id];
}

void LoadRenderTexture(RenderTextureID id, int width, int height){
  RESOURCES.renderTextures[id] = LoadRenderTexture(width, height);
}

RenderTexture2D& GetRenderTexture(RenderTextureID id){
  return RESOURCES.renderTextures[id];
}

void LoadImage(ImageID id, const Image &image){
  RESOURCES.images[id] = image;
}

Image &GetImage(ImageID id){
  return RESOURCES.images[id];
}

void Unload(){
  for(const auto& texture : RESOURCES.textures){
    UnloadTexture(texture);
  }

  for(const auto& renderTexture : RESOURCES.renderTextures){
    UnloadRenderTexture(renderTexture);
  }

  for(const auto& image : RESOURCES.images){
    UnloadImage(image);
  }
}

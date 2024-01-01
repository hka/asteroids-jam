#include "PlayerUI.h"

#include <string>

void DrawEnergyBar(Energy &energy, const Vector2 &pos, const float maxLength, const float height, const Color& color){

  float currentLength = (energy.value / energy.maxValue) * maxLength;

  Vector2 end{
    pos.x + currentLength,
    pos.y
  };

  //todo, make better looking
  DrawLineEx(pos, end, height, color);

  //todo drawText;
}

void DrawEnergyPerc(Energy &energy, const int worldWidth, const int worldHeight)
{
  std::string text = std::to_string((int)floor(energy.value)) + "%";

  float spacing = 1.f;
  int fontSize = 20;
  Vector2 textSize = MeasureTextEx(GetFontDefault(), text.c_str(), fontSize, spacing);
  float offsetX = worldWidth * 0.01f;
  float offsetY = worldHeight * 0.01f;
  Vector2 pos{
      offsetX + textSize.x,
      (float)worldHeight - offsetY - textSize.y};

  DrawTextEx(GetFontDefault(), text.c_str(), pos, fontSize, spacing, WHITE);
}

void DrawStoredAsteroids(PlayerState &player, const int worldWidth, const int worldeight){
  
  std::string text = std::to_string(player.storedAsteroids) + " / " + std::to_string(MAX_STORED_ASTEROIDS);
  float spacing = 1.f;
  int fontSize = 20;
  Vector2 textSize = MeasureTextEx(GetFontDefault(), text.c_str(), fontSize, spacing);
  float offsetX = worldWidth * 0.01f;
  float offsetY = worldeight * 0.01f;
  Vector2 pos{
      (float)worldWidth - textSize.x - offsetX,
      (float)worldeight - textSize.y - offsetY
  };

  DrawTextEx(GetFontDefault(), text.c_str(), pos, fontSize, spacing, WHITE);
}

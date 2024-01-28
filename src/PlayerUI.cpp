#include "PlayerUI.h"

#include <string>
#include <vector>

#include "globals.h"
#include "raylib.h"

void DrawEnergyBar(Energy &energy, const Rectangle& pos)
{
  float charge = (energy.value / energy.maxValue);

  Rectangle rec = pos;
  rec.y += pos.height/2.f;
  rec.height /= 2.f;
  Color bg = { 182,243,251,255 };
  DrawRectangleRec(rec, bg);

  Color bg1 { 218, 222, 222,255 };
  rec.x += rec.width/6.f;
  rec.width -= rec.width/3.f;
  rec.y += rec.height/5.f;
  rec.height -= 2*rec.height/5.f;
  DrawRectangleRec(rec, bg1);

  //todo drawText;
  {
    const char* text = "ENERGY CHARGE";
    int font_size = pos.height/2.f * 0.85f;
    float w = MeasureText(text,font_size);
    float scale = 5.2f;
    DrawTextEx(TNR, text, {pos.x, pos.y}, font_size, scale, WHITE);
  }
  {
    char charge_text[16];
    sprintf(charge_text,"%d %%",(int)std::round(charge*100));
    int font_size = rec.height;
    float w = MeasureText(charge_text,font_size);
    DrawTextEx(TNR, charge_text, {rec.x+rec.width/2.f-w/2.f, rec.y}, font_size, 1, BLACK);
  }
}

void DrawUltraBar(Energy &energy, const Rectangle& pos)
{
  float charge = (energy.value / energy.maxValue);

  Rectangle rec = pos;
  rec.y += pos.height/2.f;
  rec.height /= 2.f;
  Color bg = { 182,243,251,255 };
  DrawRectangleRec(rec, bg);

  Color bg1 { 218, 222, 222,255 };
  rec.x += rec.width/6.f;
  rec.width -= rec.width/3.f;
  rec.y += rec.height/5.f;
  rec.height -= 2*rec.height/5.f;
  DrawRectangleRec(rec, bg1);

  //todo drawText;
  {
    const char* text = "NON PLUS ULTRA";
    int font_size = pos.height/2.f * 0.85f;
    float w = MeasureText(text,font_size);
    float scale = 5.2f;
    DrawTextEx(TNR, text, {pos.x, pos.y}, font_size, scale, WHITE);
  }
  {
    char charge_text[16];
    sprintf(charge_text,"%d %%",(int)std::round(charge*100));
    int font_size = rec.height;
    float w = MeasureText(charge_text,font_size);
    DrawTextEx(TNR, charge_text, {rec.x+rec.width/2.f-w/2.f, rec.y}, font_size, 1, BLACK);
  }
  if((int)std::round(charge*100) == 100 && options.control_tip)
  {
    {
      const char* text = "PRESS SCROLL";// WHEEL TO INITIATE";
      int font_size = pos.height/2.f * 0.85f;
      float w = MeasureText(text,font_size);
      DrawTextEx(TNR, text, {pos.x+pos.width/2.f-w/2.f + font_size, pos.y - 2*font_size}, font_size, 1, bg);
    }
    {
      const char* text = "WHEEL TO INITIATE";
      int font_size = pos.height/2.f * 0.85f;
      float w = MeasureText(text,font_size);
      DrawTextEx(TNR, text, {pos.x+pos.width/2.f-w/2.f + font_size, pos.y - font_size}, font_size, 1, bg);
    }
  }
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

bool isBorder(int x, int y, int maxX, int maxY){
  return
    x == 0 || y == maxY || x == maxX;
}

void DrawEnergyShield(Energy &energy, const Rectangle &bounds){
  
  float yStartPos = bounds.y + (bounds.height / 2.f);
  float ySteps = bounds.height / 2.f;
  float width = bounds.width;
  float perc = energy.value / energy.maxValue;

  for(int y = 0; y <= ySteps; ++y){

    if(y % 5 == 0){
      width -= 6.f;
    }
    float missingX = bounds.width - width;
    float xStart = bounds.x + (missingX / 2.f);
    float y1 = yStartPos - y;
    float y2 = yStartPos + y;

    for(int x = 0; x <= width; ++x){
      float xPos = xStart + x;
      Color color;
      bool canDraw = false;
      if(isBorder(x, y, width, ySteps)){
        color = GREEN;
        canDraw = true;
      }else{
        color = BLUE;
        canDraw = x <= (width * perc);
      }

      if(canDraw){
        DrawRectangle(xPos, y1, 1.f, 1.f, color);
        DrawRectangle(xPos, y2, 1.f, 1.f, color);
      }
    }
  }

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

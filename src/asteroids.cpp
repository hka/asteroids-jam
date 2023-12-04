#include "raylib.h"

#include "screens.h"
#include "helpers.h"
#include "globals.h"

#include <memory>
#include <stdio.h>
#include <string>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

namespace
{
void UpdatePaintFrame();
void ChangeToScreen(Screen::GameScreen screen);
}

int main(void)
{
  if(FileExists("options.json"))
  {
    deserialize(options, "options.json");
  }
  else
  {
    serialize(options, "options.json");
  }

  InitWindow(options.screenWidth, options.screenHeight, PROGRAM_NAME);

  currentScreen = std::make_unique<LogoScreen>();
  if(options.skipLogo)
  {
    currentScreen = std::make_unique<MainMenuScreen>();
  }

  SetTargetFPS(options.fps);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdatePaintFrame, 0, 1);
#else

  // Main game loop
  while (!WindowShouldClose() && currentScreen != nullptr)    // Detect window close button or ESC key
  {
    UpdatePaintFrame();
  }

#endif
  CloseWindow();
  return 0;
}

namespace
{
void ChangeToScreen(Screen::GameScreen screen)
{
  if(screen == currentScreen->GetEnum())
  {
    return;
  }

  switch(screen)
  {
   case Screen::GameScreen::LOGO:
     currentScreen = std::make_unique<LogoScreen>();
     break;
   case Screen::GameScreen::MAINMENU:
     currentScreen = std::make_unique<MainMenuScreen>();
     break;
   case Screen::GameScreen::ASTEROIDS:
     currentScreen = std::make_unique<AsteroidsScreen>();
     break;
   case Screen::GameScreen::NOSCREEN:
    currentScreen = nullptr;
    break;
   default:
     break;
  }
}

void UpdatePaintFrame(void)
{
  currentScreen->Update();
  ChangeToScreen(currentScreen->Finish());

  if(currentScreen == nullptr){ return; }

  BeginDrawing();

  ClearBackground(RAYWHITE);

  currentScreen->Paint();

  EndDrawing();

}
} //anonymous namespace

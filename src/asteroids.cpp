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

#if defined(PLATFORM_WEB)
void success()
{
  printf("synced file!\n");
  if(FileExists(options_path.c_str()))
  {
    deserialize(options, options_path.c_str());
  }
}
#endif
}

int main(void)
{
#if defined(PLATFORM_WEB)
  EM_ASM(
    // Make a directory other than '/'
    FS.mkdir('/offline');
    // Then mount with IDBFS type
    FS.mount(IDBFS, {}, '/offline');

    // Then sync
    FS.syncfs(true, function (err) {
        // Error
        assert(!err);
        //ccall('success', 'v');
      });
    );
  options_path = "/offline/options.json";
#else
  options_path = "options.json";
#endif
  if(FileExists(options_path.c_str()))
  {
    deserialize(options, options_path.c_str());
  }
  else
  {
    serialize(options, options_path.c_str());
#if defined(PLATFORM_WEB)
    EM_ASM(
      FS.syncfs(function (err) {
          // Error
          assert(!err);
          //ccall('success', 'v');
        });
      );
#endif
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
   case Screen::GameScreen::OPTIONS:
    currentScreen = std::make_unique<OptionsScreen>();
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

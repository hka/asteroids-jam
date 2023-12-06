#include "raylib.h"

#include "screens.h"
#include "helpers.h"
#include "globals.h"

#include <memory>
#include <stdio.h>
#include <string>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include <atomic>
std::atomic_bool isLoading = true;
#endif

namespace
{
void UpdatePaintFrame();
void ChangeToScreen(Screen::GameScreen screen);

#if defined(PLATFORM_WEB)
extern "C" {
void success()
{
  if(FileExists(options_path.c_str()))
  {
    printf("Load options!\n");
    deserialize(options, options_path.c_str());
  }
  else
  {
    printf("synced file!\n");
    serialize(options, options_path.c_str());
    EM_ASM(
      FS.syncfs(function (err) {
          // Error
          assert(!err);
          //ccall('success', 'v');
        });
      );
  }
  isLoading = false;
}
}
  EM_JS(int, get_browser_width, (), {
      return document.documentElement.clientWidth;
    });

  EM_JS(int, get_browser_height, (), {
      return document.documentElement.clientHeight;
    });
#endif
}

int main(void)
{
#if defined(PLATFORM_WEB)
  options_path = "/offline/options.json";
  EM_ASM(
    // Make a directory other than '/'
    FS.mkdir('/offline');
    // Then mount with IDBFS type
    FS.mount(IDBFS, {}, '/offline');

    // Then sync
    FS.syncfs(true, function (err) {
        // Error
        assert(!err);
        ccall('success', 'v');
      });
    );

  while(isLoading)
  {
    emscripten_sleep(100);
  }
  options.screenWidth = get_browser_width();
  options.screenHeight = get_browser_height();
  options.screenWidth = std::min(options.screenWidth,(int)std::round(options.screenHeight*(16./9.)));
  options.screenHeight = std::min(options.screenHeight,(int)std::round(options.screenWidth/(16./9.)));
#else
  options_path = "options.json";
  if(FileExists(options_path.c_str()))
  {
    deserialize(options, options_path.c_str());
  }
  else
  {
    serialize(options, options_path.c_str());
  }
#endif

  //enforce 16:9 aspect
  options.screenWidth = std::min(options.screenWidth,(int)std::round(options.screenHeight*(16./9.)));
  options.screenHeight = std::min(options.screenHeight,(int)std::round(options.screenWidth/(16./9.)));

  InitWindow(options.screenWidth, options.screenHeight, PROGRAM_NAME);


  //Maybe can window size stuff just with raylib functions...TODO
  //SetWindowState(FLAG_WINDOW_RESIZABLE|FLAG_WINDOW_HIGHDPI);
  //printf("size: %d x %d\n",GetScreenWidth(), GetScreenHeight());
  //printf("size: %d x %d\n",GetRenderWidth(), GetRenderHeight());
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

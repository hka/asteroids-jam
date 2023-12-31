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

void LoadImageToTEXTURES(const char* path)
{
  Image art = LoadImage(path);
  Texture2D texture = LoadTextureFromImage(art);
  GenTextureMipmaps( &texture );
  SetTextureFilter(texture, TEXTURE_FILTER_TRILINEAR);
  //SetTextureFilter(texture, TEXTURE_FILTER_POINT);

  TEXTURES.push_back(texture);
}

int main(void)
{
#if defined(PLATFORM_WEB)
  options_path = "/offline/options.json";
  highscore_path = "/offline/highscore.json";
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
  if(FileExists(highscore_path.c_str()))
  {
    deserialize(highscore, highscore_path.c_str());
  }
#else
  options_path = "options.json";
  highscore_path = "highscore.json";
  if(FileExists(options_path.c_str()))
  {
    deserialize(options, options_path.c_str());
  }
  else
  {
    serialize(options, options_path.c_str());
  }
  if(FileExists(highscore_path.c_str()))
  {
    deserialize(highscore, highscore_path.c_str());
  }
#endif

  //enforce 16:9 aspect
  options.screenWidth = std::min(options.screenWidth,(int)std::round(options.screenHeight*(16./9.)));
  options.screenHeight = std::min(options.screenHeight,(int)std::round(options.screenWidth/(16./9.)));

  InitWindow(options.screenWidth, options.screenHeight, PROGRAM_NAME);


  InitAudioDevice();
  SetMasterVolume(options.master_volume);
  //main_menu_track = LoadMusicStream("data/.mp3");
  game_track = LoadMusicStream("data/The_Ultimate_Game_in-play.mp3");

  shoot_fx = LoadSound("data/laser.wav");

  //TNR = LoadFont("data/times_new_roman.ttf");
  TNR = LoadFont("data/FreeSerifItalic.ttf");

  //Maybe can window size stuff just with raylib functions...TODO
  //SetWindowState(FLAG_WINDOW_RESIZABLE|FLAG_WINDOW_HIGHDPI);
  //printf("size: %d x %d\n",GetScreenWidth(), GetScreenHeight());
  //printf("size: %d x %d\n",GetRenderWidth(), GetRenderHeight());
  currentScreen = std::make_unique<LogoScreen>();

  if(options.skipLogo || !options.first_launch)
  {
    currentScreen = std::make_unique<MainMenuScreen>();
  }
  options.first_launch = false;

  if(options.keys.empty())
  {
    SetDefaultKeys(options.keys);
  }

  SetTargetFPS(options.fps);

  // -----------------------------------------------------------------
  //Load textures
  LoadImageToTEXTURES("data/ship.png"); //0
  LoadImageToTEXTURES("data/gun.png"); //1
  LoadImageToTEXTURES("data/asteroid0.png"); //2
  LoadImageToTEXTURES("data/asteroid1.png"); //3
  LoadImageToTEXTURES("data/asteroid2.png"); //4

  LoadImageToTEXTURES("data/starfield.png"); //5

  LoadImageToTEXTURES("data/commence0.png"); //6
  LoadImageToTEXTURES("data/commence1.png"); //7

  LoadImageToTEXTURES("data/mainmenu.png"); //8

  LoadImageToTEXTURES("data/config_button0.png"); //9
  LoadImageToTEXTURES("data/config_button1.png"); //10
  LoadImageToTEXTURES("data/exit_button0.png"); //11
  LoadImageToTEXTURES("data/exit_button1.png"); //12

  LoadImageToTEXTURES("data/GameTitle.png"); //13
  LoadImageToTEXTURES("data/DirectionalButtonsHUD.png"); //14
  LoadImageToTEXTURES("data/MouseCanonHUD.png"); //15

  // -----------------------------------------------------------------

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdatePaintFrame, 0, 1);
#else

  // Main game loop
  while (!WindowShouldClose() && currentScreen != nullptr)    // Detect window close button or ESC key
  {
    UpdatePaintFrame();
  }

  //Cleanup
  for(auto& t : TEXTURES)
  {
    UnloadTexture(t);
  }

#endif
  UnloadMusicStream(game_track);
  //UnloadMusicStream(main_menu_track);
  UnloadSound(shoot_fx);
  CloseAudioDevice();
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

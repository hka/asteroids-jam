#include "globals.h"

std::unique_ptr<Screen> currentScreen;
GameOptions options;
std::string options_path;
HighScore highscore;
std::string highscore_path;
std::vector<Texture2D> TEXTURES;
std::vector<Shoot> SHOOTS;

Music main_menu_track;
Music game_track;

Sound shoot_fx;

std::mt19937 RNG = std::mt19937(4);

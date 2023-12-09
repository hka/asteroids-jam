#include "globals.h"

std::unique_ptr<Screen> currentScreen;
GameOptions options;
std::string options_path;
std::vector<Texture2D> TEXTURES;
std::vector<Shoot> SHOOTS;

std::mt19937 RNG = std::mt19937(4);

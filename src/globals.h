#ifndef ASTEROIDS_GLOBALS_H
#define ASTEROIDS_GLOBALS_H

#include <memory>
#include <vector>
#include <string>
#include <random>

#include "screens.h"

// Globals
extern GameOptions options;
extern std::string options_path;
extern HighScore highscore;
extern std::string highscore_path;
extern std::unique_ptr<Screen> currentScreen;

constexpr const char* PROGRAM_NAME = "Asteroids";

extern std::vector<Texture2D> TEXTURES;

extern Music main_menu_track;
extern Music game_track;

extern Sound shoot_fx;

extern Font TNR;

extern std::mt19937 RNG;
#endif

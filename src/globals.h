#ifndef ASTEROIDS_GLOBALS_H
#define ASTEROIDS_GLOBALS_H

#include <memory>
#include <vector>
#include <string>
#include <random>

#include "screens.h"
#include "Munition.h"

// Globals
extern GameOptions options;
extern std::string options_path;
extern std::unique_ptr<Screen> currentScreen;

constexpr const char* PROGRAM_NAME = "Asteroids";

extern std::vector<Texture2D> TEXTURES;

extern std::mt19937 RNG;
#endif

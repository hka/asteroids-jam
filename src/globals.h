#ifndef ASTEROIDS_GLOBALS_H
#define ASTEROIDS_GLOBALS_H

#include <memory>
#include <vector>
#include <string>

#include "screens.h"

// Globals
extern GameOptions options;
extern std::unique_ptr<Screen> currentScreen;

constexpr const char* PROGRAM_NAME = "Asteroids";

extern std::vector<Texture2D> TEXTURES;

#endif

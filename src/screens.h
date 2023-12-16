#ifndef ASTEROIDS_SCREENS_H
#define ASTEROIDS_SCREENS_H

#include <raylib.h>
#include <visit_struct/visit_struct.hpp>

#include <stdlib.h>
#include <string>
#include <memory>
#include <vector>
#include <random>

#include "gui_helper.h"
#include "Player.hpp"
#include "Asteroid.h"
#include "Timer.hpp"
#include "enemy.h"
#include "Munition.h"

struct GameOptions
{
  int screenWidth = 720;
  int screenHeight = 480;
  int fps = 60;
  bool skipLogo = false;
  bool godMode = true;
};
VISITABLE_STRUCT(GameOptions, screenWidth, screenHeight, fps, skipLogo);

class Screen
{
 public:
  enum class GameScreen { LOGO,
                          MAINMENU,
                          OPTIONS,
                          ASTEROIDS,
                          NOSCREEN };
  virtual ~Screen() {};
  virtual void Update() = 0;
  virtual void Paint() = 0;
  virtual GameScreen Finish() = 0;
  virtual GameScreen GetEnum() = 0;
};

class LogoScreen : public Screen
{
 public:
  LogoScreen();
  ~LogoScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::LOGO;}

 private:
  int m_framesCounter;
  Screen::GameScreen m_finishScreen;

  int m_logoPositionX;
  int m_logoPositionY;

  int m_lettersCount;

  int m_topSideRecWidth;
  int m_leftSideRecHeight;

  int m_bottomSideRecWidth;
  int m_rightSideRecHeight;

  int m_state;
  float m_alpha;
};

class MainMenuScreen : public Screen
{
 public:
  MainMenuScreen();
  ~MainMenuScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::MAINMENU;}

 private:
  GameScreen m_finishScreen;
  std::vector<Button> m_buttons;
};

class AsteroidsScreen : public Screen
{
 public:
  AsteroidsScreen();
  ~AsteroidsScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::ASTEROIDS;}

  void SetFinish(GameScreen gs){ m_finishScreen = gs; }

 private:
  void CalculateDistances(const Vector2& bound); //sets the values in the look up tables
  std::vector<float> m_player_asteroid_distance;
  std::vector<std::vector<float>> m_asteroid_asteroid_distance_squared;
  std::vector<std::vector<float>> m_enemy_asteroid_distance;

  void AsteroidAsteroidInteraction(const Vector2& bound);
  void AsteroidEnemyInteraction(const Vector2& bound);

  GameScreen m_finishScreen;
  PlayerState m_player;
  std::vector<Shoot> m_playerBullets;

  Timer m_spawnAsteroidTimer;
  std::vector<Asteroid> m_asteroids;

  Timer m_spawnEnemyTimer;
  std::vector<Enemy> m_enemies;
  std::vector<Shoot> m_enemyBullets;
};

class OptionsScreen : public Screen
{
public:
  OptionsScreen();
  ~OptionsScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::OPTIONS;}

  void SetFinish(GameScreen gs){ m_finishScreen = gs; }

private:
  GameScreen m_finishScreen;
  std::vector<Button> m_buttons;
  size_t m_toggleIntroIx;
  size_t m_toggleGodModeIx;
  std::vector<KeySelector> m_keySelector;
};


#endif

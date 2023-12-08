#include "screens.h"

#include "gui_helper.h"
#include "globals.h"

AsteroidsScreen::AsteroidsScreen():
  m_player(createPlayer({options.screenWidth / 2.f, options.screenHeight / 2.f}))
{
  m_finishScreen = Screen::GameScreen::ASTEROIDS; 
  m_spawnAsteroidTimer.start();
}

AsteroidsScreen::~AsteroidsScreen()
{
}

void AsteroidsScreen::Update()
{
  //update player
  Vector2 worldBound =  {(float)options.screenWidth, (float)options.screenHeight};
  update(m_player, worldBound);
  UpdateShoots(SHOOTS);

  if(m_spawnAsteroidTimer.getElapsed() >= 1.f){
    m_asteroids.emplace_back(CreateAsteroid(worldBound));
    m_spawnAsteroidTimer.start();
  }

  for(std::size_t i = 0; i < m_asteroids.size(); ++i){
    float asteroidRadius = m_asteroids[i].radius * 2.f;
    Vector2 asteroidBound = {worldBound.x + asteroidRadius, worldBound.y + asteroidRadius};
    UpdateAsteroid(m_asteroids[i], asteroidBound);
  }

}

void AsteroidsScreen::Paint()
{
  DrawRectangle(0, 0, options.screenWidth, options.screenHeight, BLACK);

  DrawShip(m_player);
  DrawGun(m_player);
  DrawShoots(SHOOTS);

  if(m_player.suckAttack.isOngoing){
    SuckAttack suckAttack = m_player.suckAttack;
    for(std::size_t i = 0; i < suckAttack.balls.size(); ++i){
      DrawCircle(suckAttack.balls[i].position.x, suckAttack.balls[i].position.y, suckAttack.balls[i].radius, RAYWHITE);
    }
  }

  for (std::size_t i = 0; i < m_asteroids.size(); ++i)
  {
    PaintAsteroid(m_asteroids[i]);
  }
}

Screen::GameScreen AsteroidsScreen::Finish()
{
  return m_finishScreen;
}

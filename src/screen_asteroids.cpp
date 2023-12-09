#include "screens.h"

#include "gui_helper.h"
#include "globals.h"

#include "Collision.h"

AsteroidsScreen::AsteroidsScreen():
  m_player(createPlayer({options.screenWidth / 2.f, options.screenHeight / 2.f}))
{
  m_finishScreen = Screen::GameScreen::ASTEROIDS; 
  m_spawnAsteroidTimer.start();

  m_spawnEnemyTimer.start();
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
    m_asteroids.push_back(CreateAsteroid(worldBound));
    m_spawnAsteroidTimer.start();
  }

  if(m_spawnEnemyTimer.getElapsed() >= 5.f){
    m_enemies.push_back(CreateEnemy(worldBound));
    m_spawnEnemyTimer.start();
  }

  for(std::size_t i = 0; i < m_asteroids.size(); ++i){
    float asteroidRadius = m_asteroids[i].radius * 2.f;
    Vector2 asteroidBound = {worldBound.x + asteroidRadius, worldBound.y + asteroidRadius};
    UpdateAsteroid(m_asteroids[i], asteroidBound);
  }

  for(std::size_t i = 0; i < m_enemies.size(); ++i){
    float enemyRadius = m_enemies[i].radius * 2.f;
    Vector2 enemyBound = {worldBound.x + enemyRadius, worldBound.y + enemyRadius};
    UpdateEnemy(m_enemies[i], enemyBound, m_asteroids);
  }

  //collision
  handleCollision(m_enemies, SHOOTS);
  handleCollision(m_asteroids, SHOOTS);

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

  for (size_t i = 0; i < m_enemies.size(); ++i)
  {
    PaintEnemy(m_enemies[i]);
  }
}

Screen::GameScreen AsteroidsScreen::Finish()
{
  return m_finishScreen;
}

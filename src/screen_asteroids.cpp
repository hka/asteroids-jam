#include "screens.h"

#include "gui_helper.h"
#include "globals.h"
#include "raylib_operators.h"

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

void AsteroidsScreen::CalculateDistances(const Vector2& bound)
{
  //std::vector<float> m_player_asteroid_distance;
  //std::vector<std::vector<float>> m_asteroid_asteroid_distance;
  //std::vector<std::vector<float>> m_enemy_asteroid_distance;

  //PlayerSteer m_player;
  //std::vector<Asteroid> m_asteroids;
  //std::vector<Enemy> m_enemies;
  const size_t asteroid_count = m_asteroids.size();
  m_asteroid_asteroid_distance.resize(asteroid_count, std::vector<float>(asteroid_count));
  for(size_t ii = 0; ii < asteroid_count; ++ii)
  {
    for(size_t jj = ii; jj < asteroid_count; ++jj)
    {
      const Vector2& a0 = m_asteroids[ii].data.position;
      const Vector2& a1 = m_asteroids[jj].data.position;
      float dist = CyclicDist(a0,a1,bound);
      m_asteroid_asteroid_distance[ii][jj] = dist;
      m_asteroid_asteroid_distance[jj][ii] = dist;
    }
  }

}

void AsteroidsScreen::Update()
{
  Vector2 worldBound =  {(float)options.screenWidth, (float)options.screenHeight};
  //skipping bullets for now, they will ignore the physics stuff
  //anyways
  // =================================================================
  // Calculate distance lookup
  // =================================================================
  CalculateDistances(worldBound);

  //update player
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

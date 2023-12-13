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
  const size_t asteroid_count = m_asteroids.size();
  //printf("calc asteroids %u\n",asteroid_count);
  m_asteroid_asteroid_distance_squared.resize(asteroid_count);
  for(size_t ii = 0; ii < asteroid_count; ++ii)
  {
    //must setup first as I access out of order
    m_asteroid_asteroid_distance_squared[ii].resize(asteroid_count);
  }
  for(size_t ii = 0; ii < asteroid_count; ++ii)
  {
    for(size_t jj = ii; jj < asteroid_count; ++jj)
    {
      const Vector2& a0 = m_asteroids[ii].data.position;
      const Vector2& a1 = m_asteroids[jj].data.position;
      float dist = CyclicDistSquared(a0,a1,bound);
      m_asteroid_asteroid_distance_squared[ii][jj] = dist;
      m_asteroid_asteroid_distance_squared[jj][ii] = dist;
    }
  }

  //printf("dist player\n");
  m_player_asteroid_distance.resize(asteroid_count);
  const Vector2& p0 = m_player.data.position;
  for(size_t ii = 0; ii < asteroid_count; ++ii)
  {
    const Vector2& a0 = m_asteroids[ii].data.position;
    float dist = CyclicDist(p0,a0,bound);
    m_player_asteroid_distance[ii] = dist;
  }

/*
  //some memory issue.... todo
  //printf("dist enemy\n");
  m_enemy_asteroid_distance.resize(m_enemies.size());
  for(size_t ii = 0; ii < m_enemies.size(); ++ii)
  {
    m_enemy_asteroid_distance.resize(asteroid_count);
    for(size_t jj = ii; jj < asteroid_count; ++jj)
    {
      const Vector2& e0 = m_enemies[ii].data.position;
      const Vector2& a0 = m_asteroids[jj].data.position;
      float dist = CyclicDist(e0,a0,bound);
      m_enemy_asteroid_distance[ii][jj] = dist;
    }
  }
  */
}

void AsteroidsScreen::AsteroidAsteroidInteraction(const Vector2& bound)
{
  const size_t asteroid_count = m_asteroids.size();
  for(size_t ii = 0; ii < asteroid_count; ++ii)
  {
    for(size_t jj = ii+1; jj < asteroid_count; ++jj)
    {
      Asteroid& a0 = m_asteroids[ii];
      Asteroid& a1 = m_asteroids[jj];
      float dist2 = m_asteroid_asteroid_distance_squared[ii][jj];
      if(dist2 < 1000)
      {
        float dist = std::sqrt(dist2);
        dist = dist - (a0.data.radius + a1.data.radius);
        dist2 = dist*dist;
        float k = 100000000;//InteractionConstant(ASTEROID,ASTEROID);
        //printf("%f, %f -- %f %f\n", a0.data.position.x, a0.data.position.y, a1.data.position.x, a1.data.position.y);
        Vector2 force = k*CyclicDirTo(a0.data.position,a1.data.position,bound)/dist2;
        m_asteroids[ii].data.force -= force;
        m_asteroids[jj].data.force += force;
      }
    }
  }
}

void AsteroidsScreen::Update()
{
  Vector2 worldBound =  {(float)options.screenWidth, (float)options.screenHeight};
  float dt = 1.f/GetFPS(); // more stable than GetFrameTime()?
  //skipping bullets for now, they will ignore the physics stuff
  //anyways
  // =================================================================
  // Calculate distance lookup
  // =================================================================
  CalculateDistances(worldBound);

  // =================================================================
  // Update forces
  // =================================================================
  for(std::size_t i = 0; i < m_asteroids.size(); ++i){
    //todo, currently just set to zero
    m_asteroids[i].data.force = {0, 0};
  }
  AsteroidAsteroidInteraction(worldBound);

  // =================================================================
  // Handle collision
  // =================================================================

  // =================================================================
  // Update positions (apply forces)
  // =================================================================
  for(std::size_t i = 0; i < m_asteroids.size(); ++i){
    UpdateAsteroid(m_asteroids[i], worldBound, dt);
#if 0
    printf("pos: %f, %f\n",m_asteroids[i].data.position.x,m_asteroids[i].data.position.y);
    printf("force: %f, %f\n",m_asteroids[i].data.force.x,m_asteroids[i].data.force.y);
    printf("acceleration: %f, %f\n",m_asteroids[i].data.acceleration.x,m_asteroids[i].data.acceleration.y);
    printf("velocity: %f, %f\n",m_asteroids[i].data.velocity.x,m_asteroids[i].data.velocity.y);
#endif
  }

  // =================================================================
  // Spawn enteties
  // =================================================================
  if(m_spawnAsteroidTimer.getElapsed() >= 1.f )
  {
    m_asteroids.push_back(CreateAsteroid(worldBound));
    m_spawnAsteroidTimer.start();
  }

  // =================================================================

  //update player
  update(m_player, worldBound);
  UpdateShoots(SHOOTS);



  if(m_spawnEnemyTimer.getElapsed() >= 5.f){
    m_enemies.push_back(CreateEnemy(worldBound));
    m_spawnEnemyTimer.start();
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

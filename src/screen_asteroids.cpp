#include "screens.h"

#include "gui_helper.h"
#include "globals.h"
#include "raylib_operators.h"
#include "helpers.h"
#include "Collision.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

AsteroidsScreen::AsteroidsScreen():
  m_player(createPlayer({options.screenWidth / 2.f, options.screenHeight / 2.f}))
{
  m_finishScreen = Screen::GameScreen::ASTEROIDS; 
  m_spawnAsteroidTimer.start();

  m_spawnEnemyTimer.start();

  m_namebox = InputBox(Rectangle{options.screenWidth/2.f, 3*options.screenHeight/4.f, options.screenWidth/3.f,50},12,AnchorPoint::CENTER);
  auto cancelAction = [](void* ptr){
    AsteroidsScreen* scr = (AsteroidsScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::MAINMENU;
  };
  m_namebox.cancel.action = cancelAction;
  auto confirmAction = [](void* ptr){
    AsteroidsScreen* scr = (AsteroidsScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::MAINMENU;
    Score s;
    s.score = scr->m_player.score;
    s.name = scr->m_namebox.input.text;
    highscore.scores.push_back(s);
    auto cmp = [](const Score& lh, const Score& rh){
      return lh.score > rh.score;
    };
    std::sort(highscore.scores.begin(),highscore.scores.end(),cmp);
    serialize(highscore, highscore_path.c_str());
#if defined(PLATFORM_WEB)
    EM_ASM(
      FS.syncfs(function (err) {
          assert(!err);
        });
      );
#endif
  };
  m_namebox.confirm.action = confirmAction;
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

  m_enemy_asteroid_distance.resize(m_enemies.size());
  for(size_t ii = 0; ii < m_enemies.size(); ++ii)
  {
    m_enemy_asteroid_distance[ii].resize(asteroid_count);
    for(size_t jj = 0; jj < asteroid_count; ++jj)
    {
      const Vector2& e0 = m_enemies[ii].data.position;
      const Vector2& a0 = m_asteroids[jj].data.position;
      float dist = CyclicDist(e0,a0,bound);
      m_enemy_asteroid_distance[ii][jj] = dist;
    }
  }
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

void AsteroidsScreen::AsteroidEnemyInteraction(const Vector2& bound)
{
  const size_t asteroid_count = m_asteroids.size();
  for(size_t ii = 0; ii < m_enemies.size(); ++ii)
  {
    for(size_t jj = 0; jj < asteroid_count; ++jj)
    {
      Enemy& e0 = m_enemies[ii];
      Asteroid& a0 = m_asteroids[jj];
      float dist = m_enemy_asteroid_distance[ii][jj];
      if(dist < 100)
      {
        dist = dist - (e0.data.radius + a0.data.radius);
        float dist2 = dist*dist;
        float k = 100000000;//InteractionConstant(ASTEROID,ASTEROID);
        Vector2 force = k*CyclicDirTo(e0.data.position,a0.data.position,bound)/dist2;
        e0.data.force -= force;
      }
    }
  }
}

void AsteroidsScreen::Update()
{
  if(!m_player.alive)
  {
    UpdateInputBox(GetMousePosition(), m_namebox, this);
  }
  //put stuff that should happen at end of game above this line
  //------------------------------------------------------------------
  if(!m_player.alive)
  {
    return;
  }

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
  //reset to zero
  for(std::size_t i = 0; i < m_asteroids.size(); ++i){
    m_asteroids[i].data.force = {0, 0};
  }
  AsteroidAsteroidInteraction(worldBound);
  AsteroidEnemyInteraction(worldBound);

  // =================================================================
  // Handle collision
  // =================================================================

  // =================================================================
  // Update positions (apply forces)
  // =================================================================
  for(std::size_t i = 0; i < m_asteroids.size(); ++i){
    UpdateAsteroid(m_asteroids[i], worldBound, dt);
  }
  for(std::size_t i = 0; i < m_enemies.size(); ++i){
    UpdateEnemy(m_enemies[i], worldBound, dt);
  }

  // =================================================================
  // Spawn enteties
  // =================================================================
  if(m_spawnAsteroidTimer.getElapsed() >= 5.f )
  {
    m_asteroids.push_back(CreateAsteroid(worldBound));
    m_spawnAsteroidTimer.start();
  }

  //TODO limit number of enemies to game level or something
  if(m_spawnEnemyTimer.getElapsed() >= 10.f && m_enemies.size() < 1){
    m_enemies.push_back(CreateEnemy(worldBound));
    m_spawnEnemyTimer.start();
  }

  // =================================================================

  //update player
  update(m_player, worldBound, m_playerBullets, dt);
  UpdateShoots(m_playerBullets, dt);

  //collision
  m_player.score += handleCollision(m_enemies, m_playerBullets);
  m_player.score += handleCollision(m_asteroids, m_playerBullets);

  handleCollision(m_player, m_playerBullets);
  handleCollision(m_player, m_asteroids);

}

void AsteroidsScreen::Paint()
{
  DrawRectangle(0, 0, options.screenWidth, options.screenHeight, BLACK);

  DrawShip(m_player);
  DrawGun(m_player);
  DrawShoots(m_playerBullets);

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

  //Draw score
  std::string score_text = "Score: "+std::to_string(m_player.score);
  DrawText(score_text.c_str(), 10, 10, 12, GREEN);

  if(!m_player.alive)
  {
    std::string text = "GAME OVER";
    float w = MeasureText(text.c_str(),40);
    DrawText(text.c_str(), options.screenWidth/2.f - w/2.f, options.screenHeight/2.f - 20, 40, RED);

    m_namebox.visible = true;
    PaintInputBox(m_namebox, m_frame);
    ++m_frame;
  }
}

Screen::GameScreen AsteroidsScreen::Finish()
{
  return m_finishScreen;
}

#include "screens.h"

#include "gui_helper.h"
#include "globals.h"
#include "raymath.h"

#include <iostream>

void rotateTriangle(Vector2 (&vertices)[3], const float angle);
inline void updatePosition(Vector2 (&v)[3], Vector2& velocity, const float dt);
inline void updateVelocity(const float rotation, Vector2& velocity, const float acceleration);

AsteroidsScreen::AsteroidsScreen()
{
  m_finishScreen = Screen::GameScreen::ASTEROIDS;

  m_player.position = {options.screenWidth / 2.f, options.screenHeight / 2.f};
  m_player.vertices[0] = m_player.position;
  m_player.vertices[1] = {m_player.position.x - 30.f, m_player.position.y - 10.f};
  m_player.vertices[2] = {m_player.position.x - 30.f, m_player.position.y + 10.f};

  m_player.maxAcceleration = 250.f;
  m_player.velocity = {0.f,0.f};
  m_player.currentAcceleration = 0.f;
  m_player.accelerationDecrease = 3.f;
  m_player.rotation = 0.f;
}

AsteroidsScreen::~AsteroidsScreen()
{
}

void AsteroidsScreen::Update()
{
  //todo add steering force
  if(IsKeyDown(KEY_LEFT)){
    const float angle = -0.05f;
    m_player.rotation += angle;
    rotateTriangle(m_player.vertices, angle);
  }else if(IsKeyDown(KEY_RIGHT)){
    const float angle = 0.05f;
    m_player.rotation += angle;
    rotateTriangle(m_player.vertices, angle);
  }

  if(IsKeyDown(KEY_UP)){
    m_player.currentAcceleration = m_player.maxAcceleration;
  }else{
    float newAccleration = m_player.currentAcceleration - m_player.accelerationDecrease;
    m_player.currentAcceleration = newAccleration > 0.f ? newAccleration : 0.f;
  }

  updateVelocity(m_player.rotation, m_player.velocity, m_player.currentAcceleration);
  updatePosition(m_player.vertices, m_player.velocity, GetFrameTime());

}

void AsteroidsScreen::Paint()
{
  DrawRectangle(0, 0, options.screenWidth, options.screenHeight, BLACK);
  
  DrawTriangle(
    m_player.vertices[0],
    m_player.vertices[1],
    m_player.vertices[2],
    GREEN
  );
}

void rotateTriangle(Vector2 (&v)[3], const float angle){
  Vector2 center = {(v[0].x + v[1].x + v[2].x) / 3.f, (v[0].y + v[1].y + v[2].y) / 3};

  v[0] = Vector2Add(Vector2Rotate(Vector2Subtract(v[0], center), angle), center);
  v[1] = Vector2Add(Vector2Rotate(Vector2Subtract(v[1], center), angle), center);
  v[2] = Vector2Add(Vector2Rotate(Vector2Subtract(v[2], center), angle), center);

}

inline void updateVelocity(const float rotation, Vector2& velocity, const float acceleration){
  Vector2 newVelocity = Vector2Normalize({cosf(rotation), sinf(rotation)});
  newVelocity.x *= acceleration;
  newVelocity.y *= acceleration;

  velocity = newVelocity;
}

inline void updatePosition(Vector2 (&v)[3], Vector2& velocity, const float dt){
  for(int i = 0; i < 3; ++i){
    Vector2 newPos = Vector2Add(v[i],  Vector2Scale(velocity,dt));
    v[i].x = newPos.x;
    v[i].y = newPos.y;
  }
}

Screen::GameScreen AsteroidsScreen::Finish()
{
  return m_finishScreen;
}

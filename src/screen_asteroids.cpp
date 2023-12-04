#include "screens.h"

#include <raymath.h>

#include "gui_helper.h"
#include "globals.h"

void rotateTriangle(Vector2 (&vertices)[3], float angle);

AsteroidsScreen::AsteroidsScreen()
{
  m_finishScreen = Screen::GameScreen::ASTEROIDS;

  m_player.position = {options.screenWidth / 2.f, options.screenHeight / 2.f};
  m_player.vertices[0] = m_player.position;
  m_player.vertices[1] = {m_player.position.x - 30.f, m_player.position.y - 10.f};
  m_player.vertices[2] = {m_player.position.x - 30.f, m_player.position.y + 10.f};
}

AsteroidsScreen::~AsteroidsScreen()
{
}

void AsteroidsScreen::Update()
{
  //todo add steering force
  if(IsKeyDown(KEY_LEFT)){
    float angle = m_player.rotation - 0.1f;
    rotateTriangle(m_player.vertices, angle);
  }else if(IsKeyDown(KEY_RIGHT)){
    float angle = m_player.rotation + 0.1f;
    rotateTriangle(m_player.vertices, angle);
  }
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

void rotateTriangle(Vector2 (&v)[3], float angle){
  Vector2 center = {(v[0].x + v[1].x + v[2].x) / 3.f, (v[0].y + v[1].y + v[2].y) / 3};

  v[0] = Vector2Add(Vector2Rotate(Vector2Subtract(v[0], center), angle), center);
  v[1] = Vector2Add(Vector2Rotate(Vector2Subtract(v[1], center), angle), center);
  v[2] = Vector2Add(Vector2Rotate(Vector2Subtract(v[2], center), angle), center);

}

Screen::GameScreen AsteroidsScreen::Finish()
{
  return m_finishScreen;
}

#include "screens.h"

#include "gui_helper.h"
#include "globals.h"

void rotateVertices(PlayerSteer &player, float angle);
Vector2 rotateVertice(const Vector2& point, float angle);
Vector2 addVector(Vector2 a, Vector2 b);

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
    float angle = m_player.rotation - 1.f;
    rotateVertices(m_player, angle);
  }else if(IsKeyDown(KEY_RIGHT)){
    float angle = m_player.rotation + 1.f;
    rotateVertices(m_player, angle);
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

void rotateVertices(PlayerSteer &player, float angle){

  Vector2 v1 = player.vertices[0];
  Vector2 v2 = player.vertices[1];
  Vector2 v3 = player.vertices[2];

  Vector2 center = {(v1.x + v2.x + v3.x) / 3.f, (v1.y + v2.y + v3.y) / 3};

  Vector2 v1Translated = {v1.x - center.x, v1.y - center.y};
  Vector2 v2Translated = {v2.x - center.x, v2.y - center.y};
  Vector2 v3Translated = {v3.x - center.x, v3.y - center.y};

  v1Translated = rotateVertice(v1Translated, angle);
  v2Translated = rotateVertice(v2Translated, angle);
  v3Translated = rotateVertice(v3Translated, angle);

  player.vertices[0] = addVector(v1Translated, center);
  player.vertices[1] = addVector(v2Translated, center);
  player.vertices[2] = addVector(v3Translated, center);
}

Vector2 rotateVertice(const Vector2& point, float angle){
    float rad = angle * PI / 180.0; // Convert angle to radians
    return Vector2{
        cosf(rad) * point.x - sinf(rad) * point.y,
        sinf(rad) * point.x + cosf(rad) * point.y
    };
}

Vector2 addVector(Vector2 a, Vector2 b){
  return {a.x + b.x, a.y + b.y};
}

Screen::GameScreen AsteroidsScreen::Finish()
{
  return m_finishScreen;
}

#include "screens.h"

#include "gui_helper.h"
#include "globals.h"

#include <iostream>

AsteroidsScreen::AsteroidsScreen():
  m_player(createPlayer({options.screenWidth / 2.f, options.screenHeight / 2.f}))
{
  m_finishScreen = Screen::GameScreen::ASTEROIDS; 
}

AsteroidsScreen::~AsteroidsScreen()
{
}

void AsteroidsScreen::Update()
{
  
  //update player
  rotateShip(m_player);
  accelerate(m_player);
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

Screen::GameScreen AsteroidsScreen::Finish()
{
  return m_finishScreen;
}

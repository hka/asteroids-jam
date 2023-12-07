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
  update(m_player);

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

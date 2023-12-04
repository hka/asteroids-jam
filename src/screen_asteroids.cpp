#include "screens.h"

#include "gui_helper.h"
#include "globals.h"

AsteroidsScreen::AsteroidsScreen()
{
  m_finishScreen = Screen::GameScreen::ASTEROIDS;

}

AsteroidsScreen::~AsteroidsScreen()
{
}

void AsteroidsScreen::Update()
{
}

void AsteroidsScreen::Paint()
{
  DrawRectangle(0, 0, options.screenWidth, options.screenHeight, GREEN);
}

Screen::GameScreen AsteroidsScreen::Finish()
{
  return m_finishScreen;
}

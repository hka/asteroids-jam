#include "screens.h"
#include "globals.h"

LogoScreen::LogoScreen()
{
  m_finishScreen = Screen::GameScreen::LOGO;
  m_framesCounter = 0;
  m_lettersCount = 0;

  m_logoPositionX = options.screenWidth/2 - 128;
  m_logoPositionY = options.screenHeight/2 - 128;

  m_topSideRecWidth = 16;
  m_leftSideRecHeight = 16;
  m_bottomSideRecWidth = 16;
  m_rightSideRecHeight = 16;

  m_state = 0;
  m_alpha = 1.0f;
}

LogoScreen::~LogoScreen()
{

}

void LogoScreen::Update()
{
  if (m_state == 0)                 // State 0: Top-left square corner blink logic
  {
    m_framesCounter++;

    if (m_framesCounter == 80)
    {
      m_state = 1;
      m_framesCounter = 0;      // Reset counter... will be used later...
    }
  }
  else if (m_state == 1)            // State 1: Bars animation logic: top and left
  {
    m_topSideRecWidth += 8;
    m_leftSideRecHeight += 8;

    if (m_topSideRecWidth == 256) m_state = 2;
  }
  else if (m_state == 2)            // State 2: Bars animation logic: bottom and right
  {
    m_bottomSideRecWidth += 8;
    m_rightSideRecHeight += 8;

    if (m_bottomSideRecWidth == 256) m_state = 3;
  }
  else if (m_state == 3)            // State 3: "raylib" text-write animation logic
  {
    m_framesCounter++;

    if (m_lettersCount < 10)
    {
      if (m_framesCounter/12)   // Every 12 frames, one more letter!
      {
        m_lettersCount++;
        m_framesCounter = 0;
      }
    }
    else    // When all letters have appeared, just fade out everything
    {
      if (m_framesCounter > 200)
      {
        m_alpha -= 0.02f;

        if (m_alpha <= 0.0f)
        {
          m_alpha = 0.0f;
          m_finishScreen = Screen::GameScreen::MAINMENU;   // Jump to next screen
        }
      }
    }
  }
  if(KEY_NULL != GetKeyPressed())
  {
    m_finishScreen = Screen::GameScreen::MAINMENU;   // Jump to next screen
  }
}

void LogoScreen::Paint()
{
    if (m_state == 0)         // Draw blinking top-left square corner
  {
    if ((m_framesCounter/10)%2) DrawRectangle(m_logoPositionX, m_logoPositionY, 16, 16, BLACK);
  }
  else if (m_state == 1)    // Draw bars animation: top and left
  {
    DrawRectangle(m_logoPositionX, m_logoPositionY, m_topSideRecWidth, 16, BLACK);
    DrawRectangle(m_logoPositionX, m_logoPositionY, 16, m_leftSideRecHeight, BLACK);
  }
  else if (m_state == 2)    // Draw bars animation: bottom and right
  {
    DrawRectangle(m_logoPositionX, m_logoPositionY, m_topSideRecWidth, 16, BLACK);
    DrawRectangle(m_logoPositionX, m_logoPositionY, 16, m_leftSideRecHeight, BLACK);

    DrawRectangle(m_logoPositionX + 240, m_logoPositionY, 16, m_rightSideRecHeight, BLACK);
    DrawRectangle(m_logoPositionX, m_logoPositionY + 240, m_bottomSideRecWidth, 16, BLACK);
  }
  else if (m_state == 3)    // Draw "raylib" text-write animation + "powered by"
  {
    DrawRectangle(m_logoPositionX, m_logoPositionY, m_topSideRecWidth, 16, Fade(BLACK, m_alpha));
    DrawRectangle(m_logoPositionX, m_logoPositionY + 16, 16, m_leftSideRecHeight - 32, Fade(BLACK, m_alpha));

    DrawRectangle(m_logoPositionX + 240, m_logoPositionY + 16, 16, m_rightSideRecHeight - 32, Fade(BLACK, m_alpha));
    DrawRectangle(m_logoPositionX, m_logoPositionY + 240, m_bottomSideRecWidth, 16, Fade(BLACK, m_alpha));

    DrawRectangle(options.screenWidth/2 - 112, options.screenHeight/2 - 112, 224, 224, Fade(RAYWHITE, m_alpha));

    DrawText(TextSubtext("raylib", 0, m_lettersCount), options.screenWidth/2 - 44, options.screenHeight/2 + 48, 50, Fade(BLACK, m_alpha));

    if (m_framesCounter > 20) DrawText("powered by", m_logoPositionX, m_logoPositionY - 27, 20, Fade(DARKGRAY, m_alpha));
  }
}

Screen::GameScreen LogoScreen::Finish()
{
  return m_finishScreen;
}

#include "screens.h"

#include "gui_helper.h"
#include "globals.h"

MainMenuScreen::MainMenuScreen()
{
  m_finishScreen = Screen::GameScreen::MAINMENU;

  Button b_startGame("Start", options.screenWidth/2, options.screenHeight/2- 65, 100, 20, AnchorPoint::CENTER);
  auto startAction = [](void* ptr){
    MainMenuScreen* scr = (MainMenuScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::ASTEROIDS;
  };
  b_startGame.action = startAction;

  Button b_options("Options", options.screenWidth/2, options.screenHeight/2, 100, 20, AnchorPoint::CENTER);
  PositionUnder(b_startGame, b_options);
  auto optionsAction = [](void* ptr){
    MainMenuScreen* scr = (MainMenuScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::OPTIONS;
  };
  b_options.action = optionsAction;

  Button b_exit("Exit", options.screenWidth/2, options.screenHeight/2, 100, 20, AnchorPoint::CENTER);
  PositionUnder(b_options, b_exit);
  auto exitAction = [](void* ptr){
    MainMenuScreen* scr = (MainMenuScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::NOSCREEN;
  };
  b_exit.action = exitAction;

  m_buttons.push_back(b_options);//, Screen::GameScreen::OPTIONS});
  m_buttons.push_back(b_exit);//, Screen::GameScreen::NOSCREEN});
  m_buttons.push_back(b_startGame);//, Screen::GameScreen::ASTEROIDS});
}

MainMenuScreen::~MainMenuScreen()
{
}

void MainMenuScreen::Update()
{
  Vector2 mousePoint = GetMousePosition();
  for(auto& b : m_buttons)
  {
    bool buttonPressed = CheckButton(mousePoint, b);
    if(buttonPressed)
    {
      b.action(this);
    }
  }
}

void MainMenuScreen::Paint()
{
  DrawRectangle(0, 0, options.screenWidth, options.screenHeight, GREEN);

  for(const auto& b : m_buttons)
  {
    PaintButtonWithText(b);
  }
}

Screen::GameScreen MainMenuScreen::Finish()
{
  return m_finishScreen;
}

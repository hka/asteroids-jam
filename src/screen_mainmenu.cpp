#include "screens.h"

#include "gui_helper.h"
#include "globals.h"

MainMenuScreen::MainMenuScreen()
{
  m_finishScreen = Screen::GameScreen::MAINMENU;

  Button b_startGame("Start", options.screenWidth/2, options.screenHeight/2- 65, 300, 100, AnchorPoint::CENTER);
  auto startAction = [](void* ptr){
    MainMenuScreen* scr = (MainMenuScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::ASTEROIDS;
  };
  b_startGame.action = startAction;
  b_startGame.texture0_ix = 6;
  b_startGame.texture1_ix = 7;
  b_startGame.texture2_ix = 7;

  Button b_options("Options", options.screenWidth/2, options.screenHeight/2, 300, 100, AnchorPoint::CENTER);
  PositionUnder(b_startGame, b_options);
  auto optionsAction = [](void* ptr){
    MainMenuScreen* scr = (MainMenuScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::OPTIONS;
  };
  b_options.action = optionsAction;

  Button b_exit("Exit", options.screenWidth/2, options.screenHeight/2, 300, 100, AnchorPoint::CENTER);
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
  DrawText("HIGH SCORE", 10, 10, 30, BLUE);
  for(size_t ii = 0; ii < highscore.scores.size(); ++ii)
  {
    std::string text = highscore.scores[ii].name + " - " + std::to_string(highscore.scores[ii].score);
    DrawText(text.c_str(), 10, 40 + ii*20, 15, DARKGRAY);
  }
}

Screen::GameScreen MainMenuScreen::Finish()
{
  return m_finishScreen;
}

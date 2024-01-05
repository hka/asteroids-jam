#include "screens.h"

#include "gui_helper.h"
#include "globals.h"

MainMenuScreen::MainMenuScreen()
{
  m_finishScreen = Screen::GameScreen::MAINMENU;

  int bw = options.screenWidth*252/1920.f;
  int bh = options.screenHeight*84/1080.f;
  int x = options.screenWidth*163/1920.f;
  int y = options.screenHeight*765/1080.f;
  Button b_startGame("Start", x, y, bw, bh, AnchorPoint::TOP_LEFT);
  auto startAction = [](void* ptr){
    MainMenuScreen* scr = (MainMenuScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::ASTEROIDS;
  };
  b_startGame.action = startAction;
  b_startGame.texture0_ix = 6;
  b_startGame.texture1_ix = 7;
  b_startGame.texture2_ix = 7;

  Button b_options("Options", x, y, bw, bh, AnchorPoint::CENTER);
  PositionUnder(b_startGame, b_options);
  auto optionsAction = [](void* ptr){
    MainMenuScreen* scr = (MainMenuScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::OPTIONS;
  };
  b_options.action = optionsAction;
  b_options.texture0_ix = 9;
  b_options.texture1_ix = 10;
  b_options.texture2_ix = 10;

  Button b_exit("Exit", x, y, bw, bh, AnchorPoint::CENTER);
  PositionUnder(b_options, b_exit);
  auto exitAction = [](void* ptr){
    MainMenuScreen* scr = (MainMenuScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::NOSCREEN;
  };
  b_exit.action = exitAction;
  b_exit.texture0_ix = 11;
  b_exit.texture1_ix = 12;
  b_exit.texture2_ix = 12;

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
  //DrawRectangle(0, 0, options.screenWidth, options.screenHeight, GREEN);
  //background
  {
    Texture2D te = TEXTURES[8];
    Rectangle sourceRec = { 0.0f, 0.0f, (float)te.width, (float)te.height };
    Rectangle destRec = {0, 0, (float)options.screenWidth, (float)options.screenHeight};
    Vector2 origin = { 0,0};
    float rotation = 0;
    DrawTexturePro(te, sourceRec, destRec, origin, rotation, WHITE);
  }
  //title TODO animate floating around by moving dest?
  {
    Texture2D te = TEXTURES[13];
    Rectangle sourceRec = { 0.0f, 0.0f, (float)te.width, (float)te.height };
    float scale = options.screenWidth/1920.f;
    Rectangle destRec = {(float)options.screenWidth/2, (float)options.screenHeight*0.01f, (float)te.width*scale, (float)te.height*scale};
    Vector2 origin = { destRec.width/2, 0};
    float rotation = 0;
    DrawTexturePro(te, sourceRec, destRec, origin, rotation, WHITE);
  }

  for(const auto& b : m_buttons)
  {
    PaintButtonWithText(b);
  }
  int x = options.screenWidth*1470/1920.f;
  int y = options.screenHeight*765/1080.f;
  DrawText("HIGH SCORE", x, y, 30, BLUE);
  y+=30;
  for(size_t ii = 0; ii < highscore.scores.size() && ii < 5; ++ii)
  {
    char score_text[128];
    sprintf(score_text,"%s - %09d",highscore.scores[ii].name.c_str(),(int)highscore.scores[ii].score);
    int font_size = options.screenHeight*30/1080;
    float w = MeasureText(score_text,font_size);
    DrawTextEx(TNR, score_text, {x,y+font_size*1.2*ii}, font_size, 1, GREEN);
  }


}

Screen::GameScreen MainMenuScreen::Finish()
{
  return m_finishScreen;
}

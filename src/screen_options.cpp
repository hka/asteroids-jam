#include "screens.h"

#include "gui_helper.h"
#include "globals.h"
#include "helpers.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

OptionsScreen::OptionsScreen()
{
  m_finishScreen = Screen::GameScreen::OPTIONS;

  Button b_back("Back", 10, 10, 100, 20, AnchorPoint::TOP_LEFT);
  auto backAction = [](void* ptr){
    OptionsScreen* scr = (OptionsScreen*)ptr;
    scr->m_finishScreen = Screen::GameScreen::MAINMENU;
    serialize(options, options_path.c_str()); //save options
#if defined(PLATFORM_WEB)
    EM_ASM(
      FS.syncfs(function (err) {
          assert(!err);
        });
      );
#endif
  };
  b_back.action = backAction;
  m_buttons.push_back(b_back);


  Button b_toggleIntro("Toggle intro", options.screenWidth-10, 10, 100, 20, AnchorPoint::TOP_RIGHT);
  b_toggleIntro.toggle = options.skipLogo;
  b_toggleIntro.type = Button::Type::CHECKBOX;
  auto toggleIntroAction = [](void* ptr){
    OptionsScreen* scr = (OptionsScreen*)ptr;
    options.skipLogo = !options.skipLogo;
    scr->m_buttons[scr->m_toggleIntroIx].toggle = options.skipLogo;
  };
  b_toggleIntro.action = toggleIntroAction;
  m_toggleIntroIx = m_buttons.size();
  m_buttons.push_back(b_toggleIntro);

  KeySelector increase_thrust;
  increase_thrust.pos = {options.screenWidth/4.f-25, 2.f*options.screenHeight/6.f-60, 50.f, 50.f};
  increase_thrust.text = "Increase thrust:";
  m_keySelector.push_back(increase_thrust);

  KeySelector decrease_thrust;
  decrease_thrust.pos = {options.screenWidth/4.f-25, 4.f*options.screenHeight/5.f+40, 50.f, 50.f};
  decrease_thrust.text = "Decrease thrust:";
  m_keySelector.push_back(decrease_thrust);

  KeySelector turn_cw;
  turn_cw.pos = {options.screenWidth/4.f + options.screenWidth/14.f, options.screenHeight/2.f, 50.f, 50.f};
  turn_cw.text = "Turn clockwise:";
  m_keySelector.push_back(turn_cw);

  KeySelector turn_ccw;
  turn_ccw.pos = {options.screenWidth/4.f - options.screenWidth/14.f - 50, options.screenHeight/2.f, 50.f, 50.f};
  turn_ccw.text = "Turn counter-clockwise:";
  m_keySelector.push_back(turn_ccw);
}

OptionsScreen::~OptionsScreen()
{
}

void OptionsScreen::Update()
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

void OptionsScreen::Paint()
{
  DrawRectangle(0, 0, options.screenWidth, options.screenHeight, GREEN);

  Vector2 v1 = { options.screenWidth/4.f, 2.f*options.screenHeight/6.f };
  Vector2 v2 = { options.screenWidth/4.f - options.screenWidth/14.f, 4.f*options.screenHeight/5.f };
  Vector2 v3 = { options.screenWidth/4.f + options.screenWidth/14.f, 4.f*options.screenHeight/5.f };
  DrawTriangle(v1, v2, v3, GRAY);

  DrawCircle(3.f*options.screenWidth/4.f, options.screenHeight/2.f, options.screenWidth/25.f, DARKGRAY);
  DrawLineEx({3.f*options.screenWidth/4.f, options.screenHeight/2.f},{3.f*options.screenWidth/4.f,options.screenHeight/2.f-options.screenWidth/10.f},options.screenWidth/70.f, BLACK);

  for(const auto& b : m_buttons)
  {
    PaintButtonWithText(b);
  }
  for(const auto& ks : m_keySelector)
  {
    PaintKeySelector(ks);
  }
}

Screen::GameScreen OptionsScreen::Finish()
{
  return m_finishScreen;
}

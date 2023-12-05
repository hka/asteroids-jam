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
  Vector2 v2 = { options.screenWidth/4.f - options.screenWidth/10.f, 4.f*options.screenHeight/6.f };
  Vector2 v3 = { options.screenWidth/4.f + options.screenWidth/10.f, 4.f*options.screenHeight/6.f };
  DrawTriangle(v1, v2, v3, GRAY);

  DrawCircle(3.f*options.screenWidth/4.f, options.screenHeight/2.f, options.screenWidth/20.f, DARKGRAY);
  DrawLineEx({3.f*options.screenWidth/4.f, options.screenHeight/2.f},{3.f*options.screenWidth/4.f,options.screenHeight/2.f-options.screenWidth/10.f},options.screenWidth/70.f, BLACK);

  for(const auto& b : m_buttons)
  {
    PaintButtonWithText(b);
  }
}

Screen::GameScreen OptionsScreen::Finish()
{
  return m_finishScreen;
}
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


  Button b_toggleIntro("Toggle intro", options.screenWidth-300, 10, 100, 20, AnchorPoint::TOP_RIGHT);
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

  Button b_toggleGodMode("Toggle god mode", options.screenWidth-300, 35, 100, 20, AnchorPoint::TOP_RIGHT);
  b_toggleGodMode.toggle = options.godMode;
  b_toggleGodMode.type = Button::Type::CHECKBOX;
  auto toggleGodModeAction = [](void* ptr){
    OptionsScreen* scr = (OptionsScreen*)ptr;
    options.godMode = !options.godMode;
    scr->m_buttons[scr->m_toggleGodModeIx].toggle = options.godMode;
  };
  b_toggleGodMode.action = toggleGodModeAction;
  m_toggleGodModeIx = m_buttons.size();
  m_buttons.push_back(b_toggleGodMode);

  Button b_muteFx("Mute sound effects", options.screenWidth-300, 35+25, 100, 20, AnchorPoint::TOP_RIGHT);
  b_muteFx.toggle = !options.sound_fx;
  b_muteFx.type = Button::Type::CHECKBOX;
  auto muteFxAction = [](void* ptr){
    OptionsScreen* scr = (OptionsScreen*)ptr;
    options.sound_fx = !options.sound_fx;
    scr->m_buttons[scr->m_muteFxIx].toggle = !options.sound_fx;
  };
  b_muteFx.action = muteFxAction;
  m_muteFxIx = m_buttons.size();
  m_buttons.push_back(b_muteFx);

  Button b_muteMusic("Mute sound effects", options.screenWidth-300, 35+50, 100, 20, AnchorPoint::TOP_RIGHT);
  b_muteMusic.toggle = !options.game_music;
  b_muteMusic.type = Button::Type::CHECKBOX;
  auto muteMusicAction = [](void* ptr){
    OptionsScreen* scr = (OptionsScreen*)ptr;
    options.game_music = !options.game_music;
    scr->m_buttons[scr->m_muteMusicIx].toggle = !options.game_music;
  };
  b_muteMusic.action = muteMusicAction;
  m_muteMusicIx = m_buttons.size();
  m_buttons.push_back(b_muteMusic);

  master_volume.value = std::clamp(options.master_volume,0.f,1.f);
  master_volume.min = 0.f;
  master_volume.max = 1.f;
  master_volume.state = MouseState::NO;
  master_volume.pos = {(float)options.screenWidth - 300, 120, 250, 15};

  Button b_toggleHelp("Show controller HUD", options.screenWidth-300, 145, 100, 20, AnchorPoint::TOP_RIGHT);
  b_toggleHelp.toggle = options.control_tip;
  b_toggleHelp.type = Button::Type::CHECKBOX;
  auto toggleHelpAction = [](void* ptr){
    OptionsScreen* scr = (OptionsScreen*)ptr;
    options.control_tip = !options.control_tip;
    scr->m_buttons[scr->m_toggleHelpIx].toggle = options.control_tip;
  };
  b_toggleHelp.action = toggleHelpAction;
  m_toggleHelpIx = m_buttons.size();
  m_buttons.push_back(b_toggleHelp);

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
  if(UpdateSlider(mousePoint, master_volume))
  {
    options.master_volume = master_volume.value;
    SetMasterVolume(options.master_volume);
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
  PaintSlider(master_volume);
}

Screen::GameScreen OptionsScreen::Finish()
{
  return m_finishScreen;
}

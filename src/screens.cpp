#include "screens.h"
#include "gui_helper.h"

#include <raylib.h>

void SetDefaultKeys(std::vector<Key>& keys)
{
  keys.resize((size_t)GameOptions::ControlKeyCodes::SIZE);
  //enum class ControlKeyCodes {THRUST = 0, BREAK, TURN_LEFT, TURN_RIGHT, DASH, FIRE, ABSORB, ULTRA, ALT_AIM_LEFT, ALT_AIM_RIGHT, ALT_AIM_UP, ALT_AIM_DOWN, SIZE, NONE};

  // PILOT
  keys[(size_t)GameOptions::ControlKeyCodes::THRUST].key = KEY_W;
  keys[(size_t)GameOptions::ControlKeyCodes::THRUST].is_keyboard = true;

  keys[(size_t)GameOptions::ControlKeyCodes::BREAK].key = KEY_S;
  keys[(size_t)GameOptions::ControlKeyCodes::BREAK].is_keyboard = true;

  keys[(size_t)GameOptions::ControlKeyCodes::TURN_LEFT].key = KEY_A;
  keys[(size_t)GameOptions::ControlKeyCodes::TURN_LEFT].is_keyboard = true;

  keys[(size_t)GameOptions::ControlKeyCodes::TURN_RIGHT].key = KEY_D;
  keys[(size_t)GameOptions::ControlKeyCodes::TURN_RIGHT].is_keyboard = true;

  keys[(size_t)GameOptions::ControlKeyCodes::DASH].key = KEY_SPACE;
  keys[(size_t)GameOptions::ControlKeyCodes::DASH].is_keyboard = true;

  // GUNNER
  keys[(size_t)GameOptions::ControlKeyCodes::FIRE].key = MOUSE_BUTTON_LEFT;
  keys[(size_t)GameOptions::ControlKeyCodes::FIRE].is_keyboard = false;

  keys[(size_t)GameOptions::ControlKeyCodes::ABSORB].key = MOUSE_BUTTON_RIGHT;
  keys[(size_t)GameOptions::ControlKeyCodes::ABSORB].is_keyboard = false;

  keys[(size_t)GameOptions::ControlKeyCodes::ULTRA].key = MOUSE_BUTTON_MIDDLE;
  keys[(size_t)GameOptions::ControlKeyCodes::ULTRA].is_keyboard = false;

  keys[(size_t)GameOptions::ControlKeyCodes::ALT_AIM_LEFT].key = KEY_LEFT;
  keys[(size_t)GameOptions::ControlKeyCodes::ALT_AIM_LEFT].is_keyboard = true;

  keys[(size_t)GameOptions::ControlKeyCodes::ALT_AIM_RIGHT].key = KEY_RIGHT;
  keys[(size_t)GameOptions::ControlKeyCodes::ALT_AIM_RIGHT].is_keyboard = true;

  keys[(size_t)GameOptions::ControlKeyCodes::ALT_AIM_UP].key = KEY_UP;
  keys[(size_t)GameOptions::ControlKeyCodes::ALT_AIM_UP].is_keyboard = true;

  keys[(size_t)GameOptions::ControlKeyCodes::ALT_AIM_DOWN].key = KEY_DOWN;
  keys[(size_t)GameOptions::ControlKeyCodes::ALT_AIM_DOWN].is_keyboard = true;
}

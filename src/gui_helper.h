#ifndef ASTEROIDS_GUI_HELPER
#define ASTEROIDS_GUI_HELPER

#include <raylib.h>
#include <memory>
#include <functional>
#include <string>

// =============================================================================
// enums
// =============================================================================

enum class AnchorPoint { CENTER,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    MIDDLE_LEFT,
    MIDDLE_RIGHT};

enum class MouseState {NO=0, OVER, CLICKED};

enum class Position {OVER=0, UNDER, LEFT, RIGHT};

// =============================================================================
// structs
// =============================================================================
struct Button
{
  Button(const Rectangle& r = {}, const std::string& s = "")
      : pos(r)
      , text(s)
      , state(MouseState::NO)
  {}
  Button(const char* txt, float x, float y, float w, float h, AnchorPoint anchor);
  Button(const char* txt, int x, int y, int w, int h, AnchorPoint anchor);
  Rectangle   pos;
  std::string text;
  MouseState  state;
  bool toggle = false;
  std::function<void(void*)> action;
  enum class Type {REGULAR, CHECKBOX};
  Type type = Type::REGULAR;

  int texture0_ix = -1; //default
  int texture1_ix = -1; //hover
  int texture2_ix = -1; //click
};

struct KeySelector
{
  Rectangle   pos;
  std::string text;
  MouseState  state;
  int key;
};

struct Slider
{
  Rectangle pos;
  float value;
  float min;
  float max;
  MouseState state;
};

struct Droplist
{
  Droplist(){}
  Droplist(const Vector2& position, AnchorPoint anchor,
           float width, float fontSize);
  enum class State {OPEN, CLOSED};
  Rectangle closedBox = {};
  Button    openDropDown = {};

  std::vector<std::string> lines;

  int currentIx = -1;
  State state = State::CLOSED;
  int mouseOnRowIx ;

  std::function<void(void*)> changeAction;
};

struct TextField
{
  TextField(const Rectangle& r = Rectangle(), int size = 10, Color c = LIGHTGRAY)
    : position(r)
    , fontSize(size)
    , bg(c)
    , bgDefault(c)
  {}
  Rectangle position;
  std::string text;
  int fontSize;
  Color bg;
  Color bgDefault;
};

struct InputBox
{
  InputBox() 
    : position()
    , input()
    , confirm()
    , cancel()
    , visible(false)
    , list()
    , useDroplist(false)
  {}
  InputBox(const Rectangle& r, int fontSize, AnchorPoint anchor, bool hasDroplist=false);
  Rectangle position;
  TextField input;
  Button confirm;
  Button cancel;
  bool visible;

  Droplist list;
  bool useDroplist;
};

// =============================================================================
// Gui layers, sections, dividers etc
struct Layer
{
  enum class State {HIDDEN=0, SHOWN, ACTIVE};
  State state;
  int frame = 0;
  Rectangle bounds;
  bool hideOnClickOutside = false;
  bool transparentBG = false;

  //gui elements in layer
  std::vector<Button> buttons;
  std::vector<Droplist> droplist;
  std::vector<TextField> text;
  std::vector<InputBox> inputbox;
};

// =============================================================================
void AdjustForAnchor(Rectangle& rec, AnchorPoint anchor);
void PositionUnder(const Button& ref, Button& b, float px = 5);
void PositionR(Rectangle& r, Position p, const Rectangle& r0, int px=5);

struct ButtonColors
{
  Color nomouse;
  Color mouseover;
  Color clicked;
};
void PaintButtonWithText(const Button& button, const ButtonColors& c = {LIME, GOLD, GRAY}, int fontSize = 10);
bool CheckButton(const Vector2& p, Button& button);

void PaintKeySelector(const KeySelector& ks);
void UpdateKeySelector(KeySelector& ks);

void PaintSlider(const Slider& s);
bool UpdateSlider(const Vector2& p, Slider& s);

void PaintDroplist(const Droplist& dl);
void SetDroplistIx(Droplist& dl, const std::string& key);
bool UpdateDroplist(const Vector2& mousePoint, Droplist& dl);

void PaintInputBox(const InputBox& ib, size_t frame = 0);
void UpdateInputBox(const Vector2& mousePoint, InputBox& ib, void* data);
void PaintTextField(const TextField& tf, size_t frame = 0);
void UpdateTextField(TextField& tf);

void PaintLayer(Layer& layer);
void UpdateLayer(Layer& layer, const Vector2& mousePoint, void* data,
                 bool checkOutsideClick = true);
void HideAllLayers(std::vector<Layer>& layers);
void DegradeActiveLayers(std::vector<Layer>& layers);
#endif

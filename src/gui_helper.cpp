#include "gui_helper.h"
#include "globals.h"

#include <raylib.h>

#include <cstring>

template <typename T> int sign(T val) {
  return (T(0) < val) - (val < T(0));
}

Button::Button(const char* txt, float x, float y, float w, float h, AnchorPoint anchor)
{
  pos = {x, y, w, h};
  text =  txt;
  state = MouseState::NO;
  AdjustForAnchor(pos, anchor);
}
Button::Button(const char* txt, int x, int y, int w, int h, AnchorPoint anchor)
{
    pos = { (float)x, (float)y, (float)w, (float)h };
    text = txt;
    state = MouseState::NO;
    AdjustForAnchor(pos, anchor);
}

Droplist::Droplist(const Vector2& position, AnchorPoint anchor,
                   float width, float fontSize)
  : currentIx(-1)
  , state(State::CLOSED)
  , mouseOnRowIx(-1)
{
  closedBox.x = position.x;
  closedBox.y = position.y;
  closedBox.width = width;
  float margin = 2;
  closedBox.height = fontSize + margin;
  AdjustForAnchor(closedBox, anchor);
  openDropDown = Button("V", closedBox.x+width, closedBox.y, closedBox.height, closedBox.height, AnchorPoint::TOP_RIGHT);
  lines.clear();
}

InputBox::InputBox(const Rectangle& r, int fontSize, AnchorPoint anchor, bool hasDroplist)
{
  position = r;
  AdjustForAnchor(position, anchor);
  visible = false;
  float margin = 0.05f*position.height;
  input = TextField({position.x+margin, position.y+margin, position.width-2*margin,(float)fontSize+2.f}, fontSize);
  float cx = input.position.x + input.position.width/2;
  float cy = position.y + position.height;
  int bh = 20;
  int bw = 100;
  confirm = Button("Confirm", cx-margin, cy-margin, (float)bw, (float)bh, AnchorPoint::BOTTOM_RIGHT);
  cancel = Button("Cancel", cx+margin, cy-margin, (float)bw, (float)bh, AnchorPoint::BOTTOM_LEFT);

  useDroplist = hasDroplist;
  if(hasDroplist)
  {
    float y = input.position.y + input.position.height + margin + fontSize/2.f;
    list = Droplist({cx, y}, AnchorPoint::CENTER,
                    input.position.width, (float)fontSize);
  }
}

void AdjustForAnchor(Rectangle& rec, AnchorPoint anchor)
{
  switch(anchor)
    {
    case AnchorPoint::CENTER:
      {
        rec.x -= (int)(rec.width/2.+0.5);
        rec.y -= (int)(rec.height/2.+0.5);
        break;
      }
    case AnchorPoint::TOP_LEFT:
      {
        //do nothing
        break;
      }
    case AnchorPoint::TOP_RIGHT:
      {
        rec.x -= rec.width;
        break;
      }
    case AnchorPoint::BOTTOM_LEFT:
      {
        rec.y -= rec.height;
        break;
      }
    case AnchorPoint::BOTTOM_RIGHT:
      {
        rec.x -= rec.width;
        rec.y -= rec.height;
        break;
      }
    case AnchorPoint::MIDDLE_RIGHT:
      {
        rec.x -= rec.width;
        rec.y -= (int)(rec.height/2.+0.5);
        break;
      }
    case AnchorPoint::MIDDLE_LEFT:
      {
        rec.y -= (int)(rec.height/2.+0.5);
        break;
      }
    }
}

void PositionUnder(const Button& ref, Button& b, float px)
{
  Vector2 refCenter = {ref.pos.x + ref.pos.width/2,
                       ref.pos.y + ref.pos.height};
  b.pos.x = refCenter.x - b.pos.width/2;
  b.pos.y = refCenter.y + px;
}

void PaintSlider(const Slider& s)
{
  DrawRectangleRec(s.pos, DARKGRAY);
  Rectangle grab = s.pos;
  grab.width *= 0.1;
  float pos = (s.value-s.min)/(s.max-s.min);
  grab.x = s.pos.x + pos*(s.pos.width*0.9);
  switch(s.state)
  {
   case MouseState::NO:
    DrawRectangleRec(grab, LIGHTGRAY);
    break;
   case MouseState::OVER:
   case MouseState::CLICKED:
     DrawRectangleRec(grab, GRAY);
     break;
  }
}
bool UpdateSlider(const Vector2& p, Slider& s)
{
  Rectangle grab = s.pos;
  grab.width *= 0.1;
  float pos = (s.value-s.min)/(s.max-s.min);
  grab.x = s.pos.x + pos*(s.pos.width*0.9);
  if (CheckCollisionPointRec(p, grab))
  {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
      s.state = MouseState::CLICKED;
    }
    else
    {
      s.state = MouseState::OVER;
    }
  }
  else if(!(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && s.state == MouseState::CLICKED))
  {
    s.state = MouseState::NO;
  }
  if(s.state == MouseState::CLICKED)
  {
    float new_value = (p.x - s.pos.x - grab.width/2)/(s.pos.width*0.9);
    s.value = std::clamp(new_value, s.min, s.max);
    return true;
  }
  return false;
}

void PaintButtonWithText(const Button& button, const ButtonColors& c, int fontSize)
{
  //Button
  float offh = 1;
  float offw = 1;
  bool skipText = false;
  if(button.type == Button::Type::REGULAR)
  {
    switch(button.state)
    {
    case MouseState::NO:
      if(button.texture0_ix != -1)
      {
        Texture2D te = TEXTURES[button.texture0_ix];
        Rectangle sourceRec = { 0.0f, 0.0f, (float)te.width, (float)te.height };
        Rectangle destRec = button.pos;
        Vector2 origin = { 0,0};
        float rotation = 0;
        DrawTexturePro(te, sourceRec, destRec, origin, rotation, WHITE);
        skipText = true;
      }
      else
      {
        DrawRectangleRec(button.pos, c.nomouse);
      }
      break;
    case MouseState::OVER:
      if(button.texture1_ix != -1)
      {
        Texture2D te = TEXTURES[button.texture1_ix];
        Rectangle sourceRec = { 0.0f, 0.0f, (float)te.width, (float)te.height };
        Rectangle destRec = button.pos;
        Vector2 origin = { 0,0};
        float rotation = 0;
        DrawTexturePro(te, sourceRec, destRec, origin, rotation, WHITE);
        skipText = true;
      }
      else
      {
        DrawRectangleRec(button.pos, c.mouseover);
      }
      break;
    case MouseState::CLICKED:
      if(button.texture2_ix != -1)
      {
        Texture2D te = TEXTURES[button.texture2_ix];
        Rectangle sourceRec = { 0.0f, 0.0f, (float)te.width, (float)te.height };
        Rectangle destRec = button.pos;
        Vector2 origin = { 0,0};
        float rotation = 0;
        DrawTexturePro(te, sourceRec, destRec, origin, rotation, WHITE);
        skipText = true;
      }
      else
      {
        DrawRectangleRec(button.pos, c.clicked);
      }
      break;
    }
  }
  else if(button.type == Button::Type::CHECKBOX)
  {
    Rectangle check = button.pos;
    check.width = check.height;
    DrawRectangleRec(check, c.nomouse);
    switch(button.state)
    {
    case MouseState::NO:
      break;
    case MouseState::OVER:
      DrawRectangleLinesEx(check, 2, c.mouseover);
      break;
    case MouseState::CLICKED:
      break;
    }
    if(button.toggle)
    {
      DrawLine(check.x, check.y, check.x+check.width, check.y+check.height, c.mouseover);
      DrawLine(check.x+check.width, check.y, check.x, check.y+check.height, c.mouseover);
    }
    offw += check.width + 2;
  }
  if(!skipText)
  {
    if(fontSize <= 0)
    {
      int letterCount = (int)button.text.length();
      float bw = button.pos.width;
      float bh = button.pos.height;
      fontSize = (int)(std::min<float>(bh, bw/letterCount));
      float tw = (float)MeasureText(button.text.c_str(), fontSize);
      offw = (bw - tw)/2.f+0.5f;
      offh = (bh - fontSize)/2.f+0.5f;
    }
    DrawText(button.text.c_str(), (int)(button.pos.x + offw), (int)(button.pos.y + offh), fontSize, BLACK);
    //DrawText( processText[i], (int)( toggleRecs[i].x + toggleRecs[i].width/2 - MeasureText(processText[i], 10)/2), (int) toggleRecs[i].y + 11, 10, ((i == currentProcess) || (i == mouseHoverRec)) ? DARKBLUE : DARKGRAY);
  }
}

bool CheckButton(const Vector2& p, Button& button)
{
  if (CheckCollisionPointRec(p, button.pos))
  {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
      button.state = MouseState::CLICKED;
    }
    else
    {
      button.state = MouseState::OVER;
    }
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
      //button.toggle = !button.toggle;
      return true;
    }
  }
  else
  {
    button.state = MouseState::NO;
  }
  return false;
}

void PaintKeySelector(const KeySelector& ks)
{
  DrawRectangleRec(ks.pos, LIGHTGRAY);
  DrawRectangleLinesEx(ks.pos, 2, DARKGRAY);

  int letterCount = (int)ks.text.length();
  float bh = ks.pos.height;
  int fontSize = (int)bh/4;
  float tw = (float)MeasureText(ks.text.c_str(), fontSize);
  float offw = (ks.pos.width - tw)/2.f+0.5f;
  float offh = - fontSize - 1.f;
  DrawText(ks.text.c_str(), (int)(ks.pos.x + offw), (int)(ks.pos.y + offh), fontSize, BLACK);
}
void UpdateKeySelector(KeySelector& ks)
{

}

void UpdateInputBox(const Vector2& mousePoint, InputBox& ib, void* data)
{
  if (CheckCollisionPointRec(mousePoint, ib.position) && (!ib.useDroplist || ib.list.state == Droplist::State::CLOSED))
  {
    bool buttonPressed = CheckButton(mousePoint, ib.cancel);
    if(buttonPressed)
    {
      ib.cancel.action(data);
    }
    buttonPressed = CheckButton(mousePoint, ib.confirm);
    if(buttonPressed)
    {
      ib.confirm.action(data);
    }
  }
  if(ib.visible)
  {
    UpdateTextField(ib.input);
    if(ib.useDroplist)
    {
      bool changed = UpdateDroplist(mousePoint, ib.list);
      if(changed)
      {
        ib.input.text = ib.list.lines[ib.list.currentIx];
      }
    }
  }
}

void UpdateTextField(TextField& tf)
{
  int key = GetCharPressed();
  while(key > 0)
  {
    if ((key >= 32) && (key <= 125))
    {
      tf.text.push_back((char)key);
    }
    key = GetCharPressed();
  }
  if(IsKeyPressed(KEY_BACKSPACE))
  {
    if(tf.text.size() > 0)
    {
      tf.text.pop_back();
    }
  }
}

bool UpdateDroplist(const Vector2& mousePoint, Droplist& dl)
{
  if(dl.state == Droplist::State::OPEN)
  {
    Rectangle openBox = dl.closedBox;
    float rowHeight = openBox.height;
    openBox.y += rowHeight;
    openBox.height *= dl.lines.size();
    if (CheckCollisionPointRec(mousePoint, openBox))
    {
      float adjustedMouseY = mousePoint.y - openBox.y;
      int ix = (int)(adjustedMouseY/rowHeight); //floor
      ix = ix >= (int)dl.lines.size() ? (int)dl.lines.size()-1 : ix;
      dl.mouseOnRowIx = ix;
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
      {
        dl.currentIx = ix;
        dl.mouseOnRowIx = -1;
        dl.state = Droplist::State::CLOSED;
        return true;
      }
    }
    else
    {
      dl.mouseOnRowIx = -1;
    }
  }
  else
  {
    bool buttonPressed = CheckButton(mousePoint, dl.openDropDown);
    if(buttonPressed && dl.lines.size() > 0)
    {
      dl.state = Droplist::State::OPEN;
    }
  }
  return false;
}

void PaintDroplist(const Droplist& dl)
{
  if(dl.closedBox.width == 0){ return; }
  if(dl.state == Droplist::State::OPEN)
  {
    for(size_t ii = 0; ii < dl.lines.size(); ++ii)
    {
      TextField line(dl.closedBox,(int)(dl.closedBox.height-4));
      if(dl.mouseOnRowIx == (int)ii)
      {
        line.bg = SKYBLUE;
      }
      else
      {
        line.bg = line.bgDefault;
      }
      line.position.width -= line.position.height;
      line.position.y += line.position.height*(ii+1);
      line.text = dl.lines[ii];
      PaintTextField(line, 21);
    }
  }
  DrawRectangleRec(dl.closedBox, LIGHTGRAY);
  PaintButtonWithText(dl.openDropDown, {GRAY,GRAY,GRAY}, 0);
  if(dl.currentIx >= 0)
  {
    TextField topline(dl.closedBox,(int)(dl.closedBox.height-4));
    topline.position.width -= topline.position.height;
    topline.text = dl.lines[dl.currentIx];
    PaintTextField(topline, 21);
  }
}

void PaintTextField(const TextField& tf, size_t frame)
{
  DrawRectangleRec(tf.position, tf.bg);
  DrawRectangleLines((int)tf.position.x,
                     (int)tf.position.y,
                     (int)tf.position.width,
                     (int)tf.position.height, DARKGRAY);
  int hMargin = (int)((tf.position.height - tf.fontSize)/2.f);
  int fontSize = tf.fontSize;
  const char* text = tf.text.c_str();
  int textWidth = MeasureText(text, fontSize);
  int charWidth = 0; //fontSize?
  size_t offset = 0;
  size_t textLen = tf.text.size();
  if(textWidth > tf.position.width)
  {
    for(; offset < textLen; ++offset)
    {
      textWidth = MeasureText(text + offset, fontSize);
      if(textWidth+2*hMargin+charWidth < tf.position.width)
      {
        ++offset;
        textWidth = MeasureText(text + offset, fontSize);
        break;
      }
    }
  }
  std::string text_ = tf.text + "_";
  if (((frame/20)%2) == 0)
  {
    text = text_.c_str();
  }
  DrawText(text+offset, (int)tf.position.x+hMargin, (int)tf.position.y+hMargin, fontSize, MAROON);
}

void PaintInputBox(const InputBox& ib, size_t frame)
{
  DrawRectangleRec(ib.position, GRAY);
  PaintTextField(ib.input, frame);
  PaintButtonWithText(ib.confirm);
  PaintButtonWithText(ib.cancel);
  if(ib.useDroplist)
  {
    PaintDroplist(ib.list);
  }
}

void PositionR(Rectangle& r, Position p, const Rectangle& r0, int px)
{
  r.x = r0.x + r0.width/2;
  r.y = r0.y + r0.height/2;
  switch(p)
  {
   case Position::UNDER:
     r.y = r.y + (r0.height/2 + r.height/2 + px);
     break;
   case Position::OVER:
     r.y = r.y - (r0.height/2 + r.height/2 + px);
     break;
   case Position::LEFT:
     r.x = r.x - (r0.width/2 + r.width/2 + px);
     break;
   case Position::RIGHT:
     r.x = r.x + (r0.width/2 + r.width/2 + px);
     break;
   default:
     //do nothing
     break;
  }
  AdjustForAnchor(r, AnchorPoint::CENTER);
}
void SetDroplistIx(Droplist& dl, const std::string& key)
{
  for(size_t ii = 0; ii < dl.lines.size(); ++ii)
  {
    if(!key.compare(dl.lines[ii]))
    {
      dl.currentIx = (int)ii;
      break;
    }
  }
}

void UpdateLayer(Layer& layer, const Vector2& mousePoint, void* data,
  bool checkOutsideClick)
{
  if(layer.state == Layer::State::ACTIVE && CheckCollisionPointRec(mousePoint, layer.bounds))
  {
    bool dlOpen = false;
    for(Droplist& dl : layer.droplist)
    {
      dlOpen = dlOpen || dl.state == Droplist::State::OPEN;
      bool changed = UpdateDroplist(mousePoint, dl);
      if(changed && dl.changeAction)
      {
        dl.changeAction(data);
      }
    }
    if(dlOpen) { return; }

    for(Button& b : layer.buttons)
    {
      bool buttonPressed = CheckButton(mousePoint, b);
      if(buttonPressed && b.action)
      {
        b.action(data);
      }
    }
    for(TextField& tf : layer.text)
    {
    }
    for(InputBox& ib : layer.inputbox)
    {
      UpdateInputBox(mousePoint, ib, data);
    }
    ++layer.frame;
  }
  if(checkOutsideClick)
  {
    if(layer.state != Layer::State::HIDDEN && !CheckCollisionPointRec(mousePoint, layer.bounds) && layer.hideOnClickOutside)
    {
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
      {
        layer.state = Layer::State::HIDDEN;
      }
    }
  }
}

void PaintLayer(Layer& layer)
{
  if((int)layer.state > 0)
  {
    if(!layer.transparentBG)
    {
      DrawRectangleRec(layer.bounds, DARKGRAY);
      DrawRectangleLinesEx(layer.bounds, 1.f, BLACK);
    }

    for(const Button& b : layer.buttons)
    {
      PaintButtonWithText(b);
    }
    for(const TextField& tf : layer.text)
    {
      PaintTextField(tf, 21);
    }
    for(const InputBox& ib : layer.inputbox)
    {
      PaintInputBox(ib, layer.frame);
    }
    for(const Droplist& dl : layer.droplist)
    {
      if(dl.state != Droplist::State::OPEN)
        PaintDroplist(dl);
    }
    for(const Droplist& dl : layer.droplist)
    {
      if(dl.state == Droplist::State::OPEN)
        PaintDroplist(dl);
    }
  }
}
void HideAllLayers(std::vector<Layer>& layers)
{
  for(Layer& l : layers)
  {
    l.state = Layer::State::HIDDEN;
  }
}
void DegradeActiveLayers(std::vector<Layer>& layers)
{
  for(Layer& l : layers)
  {
    if(l.state == Layer::State::ACTIVE)
    {
      l.state = Layer::State::SHOWN;
    }
  }
}

#include "screens.h"

#include "globals.h"
#include "ui/ui_layout.h"

namespace {

enum UiLayoutIds : ui::Id {
  UiRootPanel = 1,
  UiCardPanel,
  UiTitleLabel,
  UiBodyLabel,
  UiBuildLabel,
  UiButtonColumn,
  UiMainMenuButton,
  UiAnimationTestButton,
  UiAnimationEditorButton,
  UiExitGameButton,
  UiShortcutLabel,
};

ui::Style panelStyle(Color bg, Color border)
{
  ui::Style style;
  style.background = bg;
  style.border = border;
  style.borderThickness = 1.f;
  style.text = RAYWHITE;
  style.fontSize = 18;
  return style;
}

ui::Style labelStyle(Color text, int fontSize = 18)
{
  ui::Style style;
  style.text = text;
  style.fontSize = fontSize;
  return style;
}

ui::Style buttonStyle(Color bg, Color border)
{
  ui::Style style;
  style.background = bg;
  style.border = border;
  style.borderThickness = 1.f;
  style.text = RAYWHITE;
  style.fontSize = 20;
  return style;
}

ui::Layout fillColumn(float gap = 0.f, float padding = 0.f)
{
  ui::Layout layout;
  layout.axis = ui::Axis::Vertical;
  layout.width = ui::Size::grow();
  layout.height = ui::Size::grow();
  layout.gap = gap;
  layout.padding = ui::Edges::all(padding);
  return layout;
}

ui::Layout centeredCard()
{
  ui::Layout layout;
  layout.axis = ui::Axis::Vertical;
  layout.width = ui::Size::percent(0.48f);
  layout.height = ui::Size::fit();
  layout.crossAlign = ui::Align::Center;
  layout.padding = ui::Edges::all(28.f);
  layout.gap = 18.f;
  return layout;
}

ui::Layout fitLabel(float horizontalPadding = 0.f, float verticalPadding = 0.f)
{
  ui::Layout layout;
  layout.width = ui::Size::fit();
  layout.height = ui::Size::fit();
  layout.padding = ui::Edges::symmetric(horizontalPadding, verticalPadding);
  layout.crossAlign = ui::Align::Center;
  return layout;
}

ui::Layout wrappedText()
{
  ui::Layout layout;
  layout.width = ui::Size::grow();
  layout.height = ui::Size::fit();
  layout.crossAlign = ui::Align::Stretch;
  return layout;
}

ui::Layout buttonColumnLayout()
{
  ui::Layout layout;
  layout.axis = ui::Axis::Vertical;
  layout.width = ui::Size::grow();
  layout.height = ui::Size::fit();
  layout.gap = 12.f;
  return layout;
}

ui::Layout buttonLayout()
{
  ui::Layout layout;
  layout.width = ui::Size::grow();
  layout.height = ui::Size::fit();
  layout.padding = ui::Edges::symmetric(14.f, 12.f);
  return layout;
}

} // namespace

DevLandingScreen::DevLandingScreen()
{
  if (UI_FONT.texture.id != 0) m_ui.setFont(&UI_FONT);
}

DevLandingScreen::~DevLandingScreen()
{
}

void DevLandingScreen::buildUi()
{
  if (m_uiBuilt) return;

  ui::Layout rootLayout = fillColumn(0.f, 24.f);
  rootLayout.crossAlign = ui::Align::Center;
  m_ui.setRootLayout(rootLayout);

  m_ui.upsertNode(UiRootPanel, ui::kNoId, ui::NodeKind::Box, rootLayout);
  m_ui.upsertNode(UiCardPanel, UiRootPanel, ui::NodeKind::Box, centeredCard(), panelStyle(Color{16, 24, 34, 245}, Color{78, 112, 136, 255}));
  m_ui.upsertNode(UiTitleLabel, UiCardPanel, ui::NodeKind::Label, fitLabel(), labelStyle(RAYWHITE, 34), "Dev Launcher");
  m_ui.upsertNode(UiBuildLabel, UiCardPanel, ui::NodeKind::WrappedLabel, wrappedText(), labelStyle(Color{146, 170, 188, 255}, 16), "This screen is only meant for dev builds. Release-style startup still goes through the normal menu flow.");
  m_ui.upsertNode(UiShortcutLabel, UiCardPanel, ui::NodeKind::Label, wrappedText(), labelStyle(Color{150, 170, 185, 255}, 16), "Shortcuts: 1 Main Menu   2 Animation Test   3 Animation Editor   4 Exit Game");
  m_ui.upsertNode(UiButtonColumn, UiCardPanel, ui::NodeKind::Box, buttonColumnLayout());
  m_ui.upsertNode(UiMainMenuButton, UiButtonColumn, ui::NodeKind::Button, buttonLayout(), buttonStyle(Color{36, 54, 72, 255}, Color{101, 134, 160, 255}), "Main Menu");
  m_ui.upsertNode(UiAnimationTestButton, UiButtonColumn, ui::NodeKind::Button, buttonLayout(), buttonStyle(Color{36, 68, 78, 255}, Color{107, 169, 183, 255}), "Animation Test");
  m_ui.upsertNode(UiAnimationEditorButton, UiButtonColumn, ui::NodeKind::Button, buttonLayout(), buttonStyle(Color{54, 52, 82, 255}, Color{146, 132, 214, 255}), "Animation Editor");
  m_ui.upsertNode(UiExitGameButton, UiButtonColumn, ui::NodeKind::Button, buttonLayout(), buttonStyle(Color{78, 42, 48, 255}, Color{194, 112, 122, 255}), "Exit Game");
  m_uiBuilt = true;
}

void DevLandingScreen::Update()
{
  const Rectangle root{0.f, 0.f, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
  m_ui.beginFrame(root, GetMousePosition(), IsMouseButtonPressed(MOUSE_BUTTON_LEFT), IsMouseButtonDown(MOUSE_BUTTON_LEFT), IsMouseButtonReleased(MOUSE_BUTTON_LEFT));
  buildUi();
  m_ui.compute();

  if (m_ui.clicked(UiMainMenuButton) || IsKeyPressed(KEY_ONE)) {
    m_finishScreen = Screen::GameScreen::MAINMENU;
    return;
  }
  if (m_ui.clicked(UiAnimationTestButton) || IsKeyPressed(KEY_TWO)) {
    m_finishScreen = Screen::GameScreen::ANIMATION_TEST;
    return;
  }
  if (m_ui.clicked(UiAnimationEditorButton) || IsKeyPressed(KEY_THREE)) {
    m_finishScreen = Screen::GameScreen::ANIMATION_EDITOR;
    return;
  }
  if (m_ui.clicked(UiExitGameButton) || IsKeyPressed(KEY_FOUR)) {
    m_finishScreen = Screen::GameScreen::NOSCREEN;
    return;
  }
}

void DevLandingScreen::Paint()
{
  ClearBackground(Color{7, 11, 18, 255});
  m_ui.draw();
}

Screen::GameScreen DevLandingScreen::Finish()
{
  return m_finishScreen;
}

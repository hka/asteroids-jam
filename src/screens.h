#ifndef ASTEROIDS_SCREENS_H
#define ASTEROIDS_SCREENS_H

#include <raylib.h>
#include <visit_struct/visit_struct.hpp>

#include <stdlib.h>
#include <string>
#include <memory>
#include <vector>
#include <random>

#include "gui_helper.h"
#include "Player.hpp"
#include "Asteroid.h"
#include "Timer.hpp"
#include "enemy.h"
#include "Munition.h"

#include "animation/animation.h"
#include "ui/ui_layout.h"

struct GameOptions
{
  int screenWidth = 1280;
  int screenHeight = 800;
  int fps = 60;
  bool skipLogo = false;
  bool godMode = false;
  bool game_music = true;
  bool sound_fx = true;
  float master_volume = 0.5f;
  bool control_tip = true;
  bool first_launch = true;

  //Normalized by screenwidth. A value of 1 means that
  //traversing the screen takes 1 second, 0.5 means it takes 2 seconds
  //dash_distance is also given as fraction of screen size
  float player_max_velocity = 0.30;
  float player_max_acceleration = 1000000;
  float dash_distance = 0.15;
  float click_time = 0.2; //press-relase within 200ms  is click
  float speed_boost_multiplier = 1.5;
  int erasure_start_charges = 3;
  int erasure_max_charges = 3;
  float erasure_cooldown = 10; //seconds between uses

  enum class ControlKeyCodes {THRUST = 0, BREAK, TURN_LEFT, TURN_RIGHT, DASH, FIRE, ABSORB, ULTRA, ALT_AIM_LEFT, ALT_AIM_RIGHT, ALT_AIM_UP, ALT_AIM_DOWN, SPECIAL_0, SIZE, NONE};
  std::vector<Key> keys;
};
VISITABLE_STRUCT(GameOptions, screenWidth, screenHeight, fps, skipLogo, godMode, game_music, sound_fx, master_volume, control_tip, first_launch, player_max_velocity, player_max_acceleration, keys);

struct KeyMap
{
  std::vector<Key> keys;
};
VISITABLE_STRUCT(KeyMap, keys);

struct Score
{
  std::string name;
  float score;
};
VISITABLE_STRUCT(Score, name, score);
struct HighScore
{
  std::vector<Score> scores;
};
VISITABLE_STRUCT(HighScore, scores);

class Screen
{
 public:
  enum class GameScreen { LOGO,
                          DEV_LANDING,
                          MAINMENU,
                          OPTIONS,
                          ASTEROIDS,
                          ANIMATION_TEST,
                          ANIMATION_EDITOR,
                          NOSCREEN };
  virtual ~Screen() {};
  virtual void Update() = 0;
  virtual void Paint() = 0;
  virtual GameScreen Finish() = 0;
  virtual GameScreen GetEnum() = 0;
};

void SetDefaultKeys(std::vector<Key>& keys);

class LogoScreen : public Screen
{
 public:
  LogoScreen();
  ~LogoScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::LOGO;}

 private:
  int m_framesCounter;
  Screen::GameScreen m_finishScreen;

  int m_logoPositionX;
  int m_logoPositionY;

  int m_lettersCount;

  int m_topSideRecWidth;
  int m_leftSideRecHeight;

  int m_bottomSideRecWidth;
  int m_rightSideRecHeight;

  int m_state;
  float m_alpha;
};

class MainMenuScreen : public Screen
{
 public:
  MainMenuScreen();
  ~MainMenuScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::MAINMENU;}

 private:
  GameScreen m_finishScreen;
  std::vector<Button> m_buttons;
};

class AsteroidsScreen : public Screen
{
 public:
  AsteroidsScreen();
  ~AsteroidsScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::ASTEROIDS;}

  void SetFinish(GameScreen gs){ m_finishScreen = gs; }

 private:
  void CalculateDistances(const Vector2& bound); //sets the values in the look up tables
  std::vector<float> m_player_asteroid_distance;
  std::vector<std::vector<float>> m_asteroid_asteroid_distance_squared;
  std::vector<std::vector<float>> m_enemy_asteroid_distance;

  void AsteroidAsteroidInteraction(const Vector2& bound);
  void AsteroidEnemyInteraction(const Vector2& bound);

  GameScreen m_finishScreen;
  PlayerState m_player;
  std::vector<Shoot> m_playerBullets;

  Timer m_spawnAsteroidTimer;
  std::vector<Asteroid> m_asteroids;

  Timer m_spawnEnemyTimer;
  std::vector<Enemy> m_enemies;
  std::vector<Shoot> m_enemyBullets;

  InputBox m_namebox;
  size_t m_frame = 0;

  float time_in_level = 0;
  int level = 1;
};

class AnimationTestScreen : public Screen {
  public:
   AnimationTestScreen();
  ~AnimationTestScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::ANIMATION_TEST;}

   private:
    enum class TargetMode {
      Mouse,
      Random,
    };

    Vector2 randomTarget();
    bool isVisible(size_t instanceIndex) const;
    const char* visibleFilterLabel() const;

    GameScreen m_finishScreen = GameScreen::ANIMATION_TEST;
    AnimationWorld m_animationWorld;
    std::vector<std::string> m_instanceNames;
    std::vector<Vector2> m_randomTargets;
    TargetMode m_targetMode = TargetMode::Mouse;
    size_t m_visibleFilter = 0;
    bool m_paused = false;
    bool m_drawSkeleton = true;
    bool m_drawBodies = true;
    bool m_drawAppendages = true;
     bool m_drawTargets = true;
};

class DevLandingScreen : public Screen {
 public:
  DevLandingScreen();
  ~DevLandingScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::DEV_LANDING;}

 private:
  void buildUi();

  GameScreen m_finishScreen = GameScreen::DEV_LANDING;
  ui::Context m_ui;
  bool m_uiBuilt = false;
};

class AnimationEditorScreen : public Screen {
  public:
   AnimationEditorScreen();
  ~AnimationEditorScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::ANIMATION_EDITOR;}

  enum class ComponentType {
    RotateToVelocity,
    LookAt,
    BendChain,
    WaveChain,
    Pulse,
    SpringChain,
  };

  enum class RenderType {
    SpineBody,
    Appendage,
    CircleBody,
  };

  enum class PanelMode {
    Bone,
    Component,
    Render,
    Export,
  };

   private:
    struct EditableBone {
      int parent = -1;
      Transform2D bindLocal;
      std::string name;
    };

    enum class ParamTarget {
      Bone,
      Component,
      Render,
    };

    enum class ParamId {
      BoneName,
      BoneParent,
      BoneX,
      BoneY,
      BoneRotation,
      RangeStart,
      RangeCount,
      BoneIndex,
      Weight,
      TurnSpeed,
      LookWeight,
      BendMaxAngle,
      BendDistribution,
      WaveIdleAmplitude,
      WaveIdleFrequency,
      WaveMoveAmplitude,
      WaveMoveFrequency,
      WavePhaseOffset,
      PulseAmplitude,
      PulseFrequency,
      PulsePhaseOffset,
      SpringStiffness,
      SpringDamping,
      RenderWidth,
      RenderRadius,
      ColorR,
      ColorG,
      ColorB,
      ColorA,
    };

    struct EditableComponent {
      ComponentType type = ComponentType::WaveChain;
      bool enabled = true;
      BoneRange bones{0, 0};
      uint16_t bone = 0;
      float idleAmplitude = 0.05f;
      float idleFrequency = 1.0f;
      float moveAmplitude = 0.25f;
      float moveFrequency = 4.0f;
      float phaseOffset = 0.5f;
      float maxAngle = 0.25f;
      float distribution = 1.f;
      float stiffness = 80.f;
      float damping = 14.f;
      float amplitude = 0.05f;
      float frequency = 2.f;
      float turnSpeed = 2.f;
      float lookWeight = 1.f;
      float weight = 1.f;
    };

    struct EditableRenderShape {
      RenderType type = RenderType::SpineBody;
      bool enabled = true;
      BoneRange bones{0, 0};
      uint16_t bone = 0;
      float width = 12.f;
      float radius = 24.f;
      Color color{80, 220, 210, 190};
      Color outlineColor{210, 255, 245, 255};
    };

    struct EditorSnapshot {
      std::vector<EditableBone> bones;
      std::vector<EditableComponent> components;
      std::vector<EditableRenderShape> renderShapes;
      int selectedBone = -1;
      int selectedComponent = -1;
      int selectedRenderShape = -1;
      PanelMode panelMode = PanelMode::Component;
      bool snapToGrid = false;
    };

    struct ParamInput {
      ParamTarget target = ParamTarget::Component;
      ParamId id;
      const char* label = "";
      std::string text;
    };

    Skeleton buildSkeleton() const;
    void rebuildPreview();
    void ensurePreviewPose();
    int pickBone(Vector2 point) const;
    Vector2 parentLocalPosition(int parent, Vector2 worldPosition) const;
    bool isDescendant(int possibleDescendant, int ancestor) const;
    bool canReparentBone(int boneIndex, int newParent) const;
    void createBone(Vector2 worldPosition);
    void deleteSelectedBone();
    void duplicateSelectedBone();
    void importSpineSample(bool undoable = true);
    EditorSnapshot captureSnapshot() const;
    void restoreSnapshot(const EditorSnapshot& snapshot);
    void pushUndoSnapshot();
    void undo();
    void redo();
    void syncEditableRanges();
    void addComponent(ComponentType type);
    void addRenderShape(RenderType type);
    void deleteSelectedComponent();
    void duplicateSelectedComponent();
    void moveSelectedComponent(int direction);
    void deleteSelectedRenderShape();
    void cycleColorPreset();
    void syncParamInputs();
    bool applyParamInput(ParamInput& input);
    void updateParamInputs(Vector2 mouse);
    void rebuildParamInputs();
    void buildEditorUi();
    void updateEditorUi(Rectangle panel);
    bool handleEditorUi();
    void buildLeftUi();
    void updateLeftUi();
    void saveRig();
    void loadRig();
    void exportFactory() const;
    bool isEditingText() const;

    GameScreen m_finishScreen = GameScreen::ANIMATION_EDITOR;
    std::vector<EditableBone> m_bones;
    std::vector<EditableComponent> m_components;
    std::vector<EditableRenderShape> m_renderShapes;
    std::vector<ParamInput> m_paramInputs;
    std::vector<EditorSnapshot> m_undoStack;
    std::vector<EditorSnapshot> m_redoStack;
    AnimationWorld m_previewWorld;
    Transform2D m_previewTransform;
    PanelMode m_panelMode = PanelMode::Component;
    int m_selectedBone = -1;
    int m_hoveredBone = -1;
    int m_selectedComponent = 0;
    int m_selectedRenderShape = 0;
    int m_activeParamInput = -1;
    bool m_componentDropdownOpen = false;
    bool m_renderDropdownOpen = false;
    bool m_editorUiBuilt = false;
    bool m_leftUiBuilt = false;
    bool m_draggingBone = false;
    bool m_previewPaused = false;
    bool m_dirtyPreview = true;
    bool m_snapToGrid = false;
    bool m_suppressUndo = false;
    std::string m_statusText;
    ui::Context m_editorUi;
    ui::Context m_leftUi;
};

class OptionsScreen : public Screen
{
public:
  OptionsScreen();
  ~OptionsScreen();
  void Update();
  void Paint();
  GameScreen Finish();
  GameScreen GetEnum(){return Screen::GameScreen::OPTIONS;}

  void SetFinish(GameScreen gs){ m_finishScreen = gs; }

private:
  GameScreen m_finishScreen;
  std::vector<Button> m_buttons;
  size_t m_toggleIntroIx;
  size_t m_toggleGodModeIx;
  size_t m_muteMusicIx;
  size_t m_muteFxIx;
  size_t m_toggleHelpIx;
  std::vector<KeySelector> m_keySelector;
  Slider master_volume;
};


#endif

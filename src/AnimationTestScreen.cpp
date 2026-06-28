#include "animation/animation.h"
#include "raylib.h"
#include "raymath.h"
#include "screens.h"

#include <algorithm>
#include <math.h>


bool move_towards ( AnimationInstance& instance, Vector2 target, const float dt ) {
  // todo refactor into component
  float maxSpeed = 200.f;
  float slowRadius = 250.f;
  float arriveRadius = 35.f;

  Vector2 diff = target - instance.transform.position;

  float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

  instance.previousVelocity = instance.velocity;
  
  if ( dist > arriveRadius ) {
    Vector2 dir = diff / dist;

    float speedFactor = std::min(dist / slowRadius, 1.f);
    float speed = maxSpeed * speedFactor;
    float step = speed * dt;

    if (step >= dist - arriveRadius) {
      instance.transform.position += dir * (dist - arriveRadius);
      instance.velocity = {0,0};
      instance.moveSpeed = 0.f;
      instance.moveAmount = 0.f;
      instance.turnAmount = 0.f;
      return true;
    }

    instance.velocity = dir * speed;
    instance.moveSpeed = speed;
    instance.moveAmount = std::clamp(speed / maxSpeed, 0.f, 1.f);

    Vector2 facing = { std::cos(instance.transform.rotation), std::sin(instance.transform.rotation) };
    instance.turnAmount = std::clamp(facing.x * dir.y - facing.y * dir.x, -1.f, 1.f);

    instance.transform.position += dir * step;
  } else {
    instance.velocity = {0,0};
    instance.moveSpeed = 0.f;
    instance.moveAmount = 0.f;
    instance.turnAmount = 0.f;
    return true;
  }

  return false;
}

AnimationTestScreen::AnimationTestScreen()
{
  createFish(m_animationWorld);
  m_instanceNames.push_back("Fish");
  createSpineShip(m_animationWorld);  
  m_instanceNames.push_back("Spine Ship");
  createMantaShip(m_animationWorld);  
  m_instanceNames.push_back("Manta Ship");
  createSerpentShip(m_animationWorld);  
  m_instanceNames.push_back("Serpent Ship");
  createEngineCreature(m_animationWorld);  
  m_instanceNames.push_back("Engine Creature");
  createOrbTentacleCreature(m_animationWorld);  
  m_instanceNames.push_back("Orb Tentacles");

  m_randomTargets.reserve(m_animationWorld.instances.size());
  for (size_t i = 0; i < m_animationWorld.instances.size(); ++i) {
    m_randomTargets.push_back(randomTarget());
  }
}

AnimationTestScreen::~AnimationTestScreen()
{
  
}

Vector2 AnimationTestScreen::randomTarget()
{
  constexpr int margin = 80;
  const int maxX = std::max(margin, GetScreenWidth() - margin);
  const int maxY = std::max(margin, GetScreenHeight() - margin);

  return {
    static_cast<float>(GetRandomValue(margin, maxX)),
    static_cast<float>(GetRandomValue(margin, maxY))
  };
}

bool AnimationTestScreen::isVisible(size_t instanceIndex) const
{
  return m_visibleFilter == 0 || m_visibleFilter == instanceIndex + 1;
}

const char* AnimationTestScreen::visibleFilterLabel() const
{
  if (m_visibleFilter == 0) return "All";

  const size_t instanceIndex = m_visibleFilter - 1;
  if (instanceIndex >= m_instanceNames.size()) return "Unknown";

  return m_instanceNames[instanceIndex].c_str();
}

void AnimationTestScreen::Update()
{
  float dt = GetFrameTime();

  if (IsKeyPressed(KEY_SPACE)) {
    m_targetMode = m_targetMode == TargetMode::Mouse ? TargetMode::Random : TargetMode::Mouse;
  }

  if (IsKeyPressed(KEY_F2)) {
    m_finishScreen = Screen::GameScreen::ANIMATION_EDITOR;
    return;
  }

  if (IsKeyPressed(KEY_P)) {
    m_paused = !m_paused;
  }

  if (IsKeyPressed(KEY_TAB)) {
    m_visibleFilter = (m_visibleFilter + 1) % (m_animationWorld.instances.size() + 1);
  }

  if (IsKeyPressed(KEY_S)) {
    m_drawSkeleton = !m_drawSkeleton;
  }

  if (IsKeyPressed(KEY_B)) {
    m_drawBodies = !m_drawBodies;
  }

  if (IsKeyPressed(KEY_A)) {
    m_drawAppendages = !m_drawAppendages;
  }

  if (IsKeyPressed(KEY_T)) {
    m_drawTargets = !m_drawTargets;
  }

  if (IsKeyPressed(KEY_R)) {
    for (size_t i = 0; i < m_animationWorld.instances.size(); ++i) {
      if (!isVisible(i)) continue;

      AnimationInstance& instance = m_animationWorld.instances[i];
      Vector2 recoilDir = { -std::cos(instance.transform.rotation), -std::sin(instance.transform.rotation) };
      triggerRecoil(instance, recoilDir, 1.f);
      triggerHitReaction(instance, 1.f);
      triggerPulseSpike(instance, 1.f);
      triggerTentacleFlinch(instance, 1.f);
      triggerColorFlash(instance, 1.f);
    }
  }

  if (m_paused) return;

  if (m_randomTargets.size() < m_animationWorld.instances.size()) {
    m_randomTargets.resize(m_animationWorld.instances.size());
    for (Vector2& target : m_randomTargets) {
      if (target.x == 0.f && target.y == 0.f) {
        target = randomTarget();
      }
    }
  }

  for (size_t i = 0; i < m_animationWorld.instances.size(); ++i) {
    if (!isVisible(i)) continue;

    AnimationInstance& instance = m_animationWorld.instances[i];
    Vector2 target = GetScreenToWorld2D(GetMousePosition(), {{0,0}, {0,0}, 0.f, 1.f});

    if (m_targetMode == TargetMode::Random) {
      target = m_randomTargets[i];
    }

    instance.aimTarget = target;

    const bool reachedTarget = move_towards(instance, target, dt);
    if (reachedTarget && m_targetMode == TargetMode::Random) {
      m_randomTargets[i] = randomTarget();
    }
  }

  updateAnimationWorld(m_animationWorld, dt);
}

void AnimationTestScreen::Paint()
{
  for (size_t i = 0; i < m_animationWorld.instances.size(); ++i ) {
    if (!isVisible(i)) continue;

    const AnimationInstance& instance = m_animationWorld.instances[i];
    if (m_drawAppendages) {
      for (uint32_t appendageId : instance.appendageIds) {
        drawAppendage(m_animationWorld.appendages[appendageId], instance.pose, instance.colorFlashAmount);
      }
    }

    if (m_drawBodies && instance.hasSpineBody) {
      drawSpineBody(m_animationWorld.spineBodies[instance.spineBodyId], instance.pose, instance.colorFlashAmount);
    }

    if (m_drawBodies && instance.hasCircleBody) {
      drawCircleBody(m_animationWorld.circleBodies[instance.circleBodyId], instance.pose, instance.colorFlashAmount);
    }

    if (m_drawSkeleton) {
      drawSkeleton(m_animationWorld.skeletons[instance.skeletonId], instance.pose);
    }

    if (i < m_instanceNames.size()) {
      DrawText(m_instanceNames[i].c_str(), static_cast<int>(instance.transform.position.x) - 45, static_cast<int>(instance.transform.position.y) - 55, 14, RAYWHITE);
    }
  }

  if (m_drawTargets && m_targetMode == TargetMode::Random) {
    for (size_t i = 0; i < m_randomTargets.size(); ++i) {
      if (!isVisible(i)) continue;

      const Vector2& target = m_randomTargets[i];
      DrawCircleLines(static_cast<int>(target.x), static_cast<int>(target.y), 12.f, ORANGE);
    }
  }

  const char* modeText = m_targetMode == TargetMode::Mouse
    ? "Mode: mouse follow (Space: random targets)"
    : "Mode: random targets (Space: mouse follow)";
  DrawText(modeText, 20, 20, 20, RAYWHITE);
  DrawText(TextFormat("Filter: %s | %s", visibleFilterLabel(), m_paused ? "Paused" : "Running"), 20, 45, 18, RAYWHITE);
  DrawText(TextFormat("F2 editor | Tab filter | R react | P pause | S skeleton:%s | B body:%s | A appendages:%s | T targets:%s",
    m_drawSkeleton ? "on" : "off",
    m_drawBodies ? "on" : "off",
    m_drawAppendages ? "on" : "off",
    m_drawTargets ? "on" : "off"), 20, 68, 16, LIGHTGRAY);


  // for (size_t i = 0; i < m_animationWorld.instances.size(); ++i ) {
  //   drawBoneDirection(m_animationWorld.instances[i].pose);
  // }
}

Screen::GameScreen AnimationTestScreen::Finish()
{
  return m_finishScreen;
}

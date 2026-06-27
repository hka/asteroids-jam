#include "animation/animation.h"
#include "raylib.h"
#include "raymath.h"
#include "screens.h"

#include <math.h>


void move_towards ( AnimationInstance& instance, Vector2 target, const float dt ) {
  // todo refactor into component
  float maxSpeed = 200.f;
  float slowRadius = 250.f;
  float arriveRadius = 10.f;

  Vector2 diff = target - instance.transform.position;

  float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

  
  if ( dist > arriveRadius ) {
    Vector2 dir = diff / dist;

    float speedFactor = std::min(dist / slowRadius, 1.f);
    float speed = maxSpeed * speedFactor;
    float step = speed * dt;

    if (step > dist - arriveRadius) {
      step = dist - arriveRadius;
    }

    instance.velocity = dir * speed;

    instance.transform.position += dir * step;

    instance.speed = speed;
  } else {
    instance.velocity = {0,0};
    instance.speed = 0;
  }
}

AnimationTestScreen::AnimationTestScreen()
{
  createFish(m_animationDb, m_animationInstances);  
}

AnimationTestScreen::~AnimationTestScreen()
{
  
}

void AnimationTestScreen::Update()
{
  float dt = GetFrameTime();

  const float speed = 0.5f;
  for (AnimationInstance& instance : m_animationInstances) {
    move_towards(instance, GetScreenToWorld2D(GetMousePosition(), {{0,0}, {0,0}, 0.f, 1.f}), dt);
    updateAnimation(m_animationDb, instance, dt);
  }
}

void AnimationTestScreen::Paint()
{
  for (size_t i = 0; i < m_animationInstances.size(); ++i ) {
    drawSkeleton(m_animationDb.skeletons[i], m_animationInstances[i].pose);
  }
  

  // for (size_t i = 0; i < m_animationInstances.size(); ++i ) {
  //   drawBoneDirection(m_animationInstances[i].pose);
  // }
}

Screen::GameScreen AnimationTestScreen::Finish()
{
  return Screen::GameScreen::ANIMATION_TEST;
}


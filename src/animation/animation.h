#ifndef ASTEROIDS_ANIMATION_H_
#define ASTEROIDS_ANIMATION_H_

#include "raylib.h"
#include <cstdint>
#include <vector>


struct Transform2D {
  Vector2 position;
  float rotation = 0.0f;
  Vector2 scale{1.f, 1.f};
};

struct Bone {
  int parent = -1;
  Transform2D bindLocal;
};

struct Skeleton {
  std::vector<Bone> bones;
};

struct Pose {
  std::vector<Transform2D> local;
  std::vector<Transform2D> world;
};

enum class ProcedureType {
  WaveChain,
  LookAt,
  Spring,
  RotateToVelocity,
};

struct Procedure {
  ProcedureType type;
  uint32_t paramIndex;
};

struct BoneRange {
  uint16_t start = 0;
  uint16_t count = 0;
};

struct WaveChainParams {
  BoneRange bones;
  float idleAmplitude = 0.05f;
  float idleFrequency = 1.0f;

  float moveAmplitude = 0.25f;
  float moveFrequency = 4.0f;

  float phaseOffset = 0.5f;
};

struct LookAtParams {
  uint16_t bone = 0;
  Vector2 target;
  float weight = 1.0f;
};

struct RotateToVelocityParams {
  float turnSpeed = 1.f;
};

struct AnimationDatabase {
  std::vector<Skeleton> skeletons;
  std::vector<WaveChainParams> waveChains;
  std::vector<LookAtParams> lookAts;
  std::vector<RotateToVelocityParams> rotateToVelocity;
};

struct AnimationInstance {
  uint32_t skeletonId = 0;
  Pose pose;
  Transform2D transform;
  std::vector<Procedure> procedures;

  float time = 0.f;
  float speed = 0.f;
  Vector2 velocity;
  Vector2 aimTarget;
};

void updateAnimation( AnimationDatabase& db, AnimationInstance& instance, float dt);

void applyWaveChain( const WaveChainParams& params, AnimationInstance& instance);
void applyRotateToVelocity( const RotateToVelocityParams& params, AnimationInstance& instance, const float dt);

Transform2D combine(const Transform2D& parent, const Transform2D& child);
void solveWorldPose( const Skeleton& skeleton, const Transform2D& rootTransform, Pose& pose );
void resetToBindPose(const Skeleton& skeleton, Pose& pose);
void resetToBindPose(const Skeleton& skeleton, Pose& pose);

// ---- FISH ---- //
void createFish(AnimationDatabase& db, std::vector<AnimationInstance>& instances);
Skeleton createFishSkeleton();

// ---- Debug draw ---- //
void drawSkeleton(const Skeleton& skeleton, const Pose& pose);
void drawBoneDirection(const Pose& pose);

#endif

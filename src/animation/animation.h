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

struct BendChainParams {
  BoneRange bones;
  float maxAngle = 0.25f;
  float distribution = 1.0f;
};

struct SpringChainParams {
  BoneRange bones;
  float stiffness = 80.f;
  float damping = 14.f;
};

struct PulseParams {
  BoneRange bones;
  float amplitude = 0.05f;
  float frequency = 2.f;
  float phaseOffset = 0.4f;
};

struct LookAtParams {
  uint16_t bone = 0;
  Vector2 target{};
  float weight = 1.0f;
};

struct RotateToVelocityParams {
  float turnSpeed = 1.f;
};

struct RotateToVelocityComponent {
  uint32_t instanceId = 0;
  RotateToVelocityParams params;
  float weight = 1.f;
};

struct LookAtComponent {
  uint32_t instanceId = 0;
  LookAtParams params;
  float weight = 1.f;
};

struct BendChainComponent {
  uint32_t instanceId = 0;
  BendChainParams params;
  float weight = 1.f;
};

struct WaveChainComponent {
  uint32_t instanceId = 0;
  WaveChainParams params;
  float weight = 1.f;
};

struct PulseComponent {
  uint32_t instanceId = 0;
  PulseParams params;
  float weight = 1.f;
};

struct SpringChainComponent {
  uint32_t instanceId = 0;
  SpringChainParams params;
  float weight = 1.f;
};

struct SpineBodyRenderParams {
  BoneRange bones;
  Color color = WHITE;
  Color outlineColor = GRAY;
  std::vector<float> widths;
};

struct AppendageRenderParams {
  BoneRange bones;
  Color color = WHITE;
  Color outlineColor = GRAY;
  std::vector<float> widths;
};

struct CircleBodyRenderParams {
  uint16_t bone = 0;
  float radius = 20.f;
  Color color = WHITE;
  Color outlineColor = GRAY;
};

struct AnimationArchetype {
  const char* name = "";
  uint32_t skeletonId = 0;
  std::vector<uint32_t> rotateToVelocityIds;
  std::vector<uint32_t> lookAtIds;
  std::vector<uint32_t> bendChainIds;
  std::vector<uint32_t> waveChainIds;
  std::vector<uint32_t> pulseIds;
  std::vector<uint32_t> springChainIds;
  bool hasSpineBody = false;
  uint32_t spineBodyId = 0;
  bool hasCircleBody = false;
  uint32_t circleBodyId = 0;
  std::vector<uint32_t> appendageIds;
};

struct AnimationInstance {
  uint32_t skeletonId = 0;
  bool hasSpineBody = false;
  uint32_t spineBodyId = 0;
  bool hasCircleBody = false;
  uint32_t circleBodyId = 0;
  std::vector<uint32_t> appendageIds;
  Pose pose;
  Transform2D transform;
  std::vector<float> springRotation;
  std::vector<float> springVelocity;

  float time = 0.f;
  float moveSpeed = 0.f;
  float moveAmount = 0.f;
  float turnAmount = 0.f;
  Vector2 velocity;
  Vector2 previousVelocity;
  Vector2 aimTarget;

  Vector2 recoilOffset;
  Vector2 recoilVelocity;
  float recoilTimer = 0.f;
  float recoilStrength = 0.f;
  float hitWobbleTimer = 0.f;
  float hitWobbleStrength = 0.f;
  float pulseSpikeTimer = 0.f;
  float pulseSpikeStrength = 0.f;
  float flinchTimer = 0.f;
  float flinchStrength = 0.f;
  float colorFlashTimer = 0.f;
  float colorFlashStrength = 0.f;
  float colorFlashAmount = 0.f;
};

struct AnimationWorld {
  std::vector<AnimationArchetype> archetypes;
  std::vector<Skeleton> skeletons;
  std::vector<AnimationInstance> instances;
  std::vector<RotateToVelocityComponent> rotateToVelocity;
  std::vector<LookAtComponent> lookAts;
  std::vector<BendChainComponent> bendChains;
  std::vector<WaveChainComponent> waveChains;
  std::vector<PulseComponent> pulses;
  std::vector<SpringChainComponent> springChains;
  std::vector<SpineBodyRenderParams> spineBodies;
  std::vector<AppendageRenderParams> appendages;
  std::vector<CircleBodyRenderParams> circleBodies;
};

uint32_t addWaveChain(AnimationWorld& world, uint32_t instanceId, WaveChainParams params, float weight = 1.f);
uint32_t addSpringChain(AnimationWorld& world, uint32_t instanceId, SpringChainParams params, float weight = 1.f);
uint32_t addBendChain(AnimationWorld& world, uint32_t instanceId, BendChainParams params, float weight = 1.f);
uint32_t addPulse(AnimationWorld& world, uint32_t instanceId, PulseParams params, float weight = 1.f);
uint32_t addLookAt(AnimationWorld& world, uint32_t instanceId, LookAtParams params, float weight = 1.f);
uint32_t addRotateToVelocity(AnimationWorld& world, uint32_t instanceId, RotateToVelocityParams params, float weight = 1.f);
uint32_t addSpineBody(AnimationWorld& world, SpineBodyRenderParams params);
uint32_t addAppendage(AnimationWorld& world, AppendageRenderParams params);
uint32_t addCircleBody(AnimationWorld& world, CircleBodyRenderParams params);
uint32_t addAnimationArchetype(AnimationWorld& world, AnimationArchetype archetype);

void updateAnimationWorld(AnimationWorld& world, float dt);
void updateRotateToVelocity(AnimationWorld& world, float dt);
void updateLookAts(AnimationWorld& world);
void updateBendChains(AnimationWorld& world);
void updateWaveChains(AnimationWorld& world);
void updatePulses(AnimationWorld& world);
void updateSpringChains(AnimationWorld& world, float dt);

void triggerRecoil(AnimationInstance& instance, Vector2 direction, float strength);
void triggerHitReaction(AnimationInstance& instance, float strength);
void triggerPulseSpike(AnimationInstance& instance, float strength);
void triggerTentacleFlinch(AnimationInstance& instance, float strength);
void triggerColorFlash(AnimationInstance& instance, float strength);
void triggerRecoil(AnimationWorld& world, uint32_t instanceId, Vector2 direction, float strength);
void triggerHitReaction(AnimationWorld& world, uint32_t instanceId, float strength);
void triggerPulseSpike(AnimationWorld& world, uint32_t instanceId, float strength);
void triggerTentacleFlinch(AnimationWorld& world, uint32_t instanceId, float strength);
void triggerColorFlash(AnimationWorld& world, uint32_t instanceId, float strength);
Color applyColorFlash(Color color, float flashAmount);

void applyWaveChain( const WaveChainParams& params, AnimationInstance& instance, float weight);
void applyBendChain( const BendChainParams& params, AnimationInstance& instance, float weight);
void applySpringChain( const Skeleton& skeleton, const SpringChainParams& params, AnimationInstance& instance, const float dt, float weight);
void applyPulse( const PulseParams& params, AnimationInstance& instance, float weight);
void applyLookAt( const Skeleton& skeleton, const LookAtParams& params, AnimationInstance& instance, float weight);
void applyRotateToVelocity( const RotateToVelocityParams& params, AnimationInstance& instance, const float dt, float weight);

Transform2D combine(const Transform2D& parent, const Transform2D& child);
void solveWorldPose( const Skeleton& skeleton, const Transform2D& rootTransform, Pose& pose );
void resetToBindPose(const Skeleton& skeleton, Pose& pose);

// ---- FISH ---- //
void createFish(AnimationWorld& world);
Skeleton createFishSkeleton();

// ---- SPINE SHIP ---- //
void createSpineShip(AnimationWorld& world);
Skeleton createSpineShipSkeleton();

// ---- MANTA SHIP ---- //
void createMantaShip(AnimationWorld& world);
Skeleton createMantaShipSkeleton();

// ---- SERPENT SHIP ---- //
void createSerpentShip(AnimationWorld& world);
Skeleton createSerpentShipSkeleton();

// ---- ENGINE CREATURE ---- //
void createEngineCreature(AnimationWorld& world);
Skeleton createEngineCreatureSkeleton();

// ---- ORB TENTACLE CREATURE ---- //
void createOrbTentacleCreature(AnimationWorld& world);
Skeleton createOrbTentacleCreatureSkeleton();

// ---- Debug draw ---- //
void drawAppendage(const AppendageRenderParams& params, const Pose& pose, float flashAmount = 0.f);
void drawCircleBody(const CircleBodyRenderParams& params, const Pose& pose, float flashAmount = 0.f);
void drawSpineBody(const SpineBodyRenderParams& params, const Pose& pose, float flashAmount = 0.f);
void drawSkeleton(const Skeleton& skeleton, const Pose& pose);
void drawBoneDirection(const Pose& pose);

#endif

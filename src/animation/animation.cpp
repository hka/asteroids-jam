#include "animation.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace {

constexpr float kRecoilDuration = 0.28f;
constexpr float kHitWobbleDuration = 0.34f;
constexpr float kPulseSpikeDuration = 0.24f;
constexpr float kFlinchDuration = 0.22f;
constexpr float kColorFlashDuration = 0.18f;

float safeDivide(float value, float divisor) {
  return divisor > 0.f ? value / divisor : 0.f;
}

Vector2 normalizedOrZero(Vector2 v) {
  const float length = std::sqrt(v.x * v.x + v.y * v.y);
  if (length <= 0.0001f) return {0.f, 0.f};
  return {v.x / length, v.y / length};
}

void updateReactionTimers(AnimationInstance& instance, float dt) {
  instance.recoilTimer = std::max(0.f, instance.recoilTimer - dt);
  instance.hitWobbleTimer = std::max(0.f, instance.hitWobbleTimer - dt);
  instance.pulseSpikeTimer = std::max(0.f, instance.pulseSpikeTimer - dt);
  instance.flinchTimer = std::max(0.f, instance.flinchTimer - dt);
  instance.colorFlashTimer = std::max(0.f, instance.colorFlashTimer - dt);

  instance.recoilOffset.x += instance.recoilVelocity.x * dt;
  instance.recoilOffset.y += instance.recoilVelocity.y * dt;
  const float damping = std::pow(0.04f, dt);
  instance.recoilVelocity.x *= damping;
  instance.recoilVelocity.y *= damping;

  if (instance.recoilTimer <= 0.f) {
    const float returnAmount = std::min(1.f, dt * 14.f);
    instance.recoilOffset.x += (0.f - instance.recoilOffset.x) * returnAmount;
    instance.recoilOffset.y += (0.f - instance.recoilOffset.y) * returnAmount;
  }

  const float flashT = safeDivide(instance.colorFlashTimer, kColorFlashDuration);
  instance.colorFlashAmount = std::clamp(flashT * instance.colorFlashStrength, 0.f, 1.f);
}

void applyReactionPose(AnimationInstance& instance) {
  const size_t boneCount = instance.pose.local.size();
  if (boneCount == 0) return;

  const float wobbleT = safeDivide(instance.hitWobbleTimer, kHitWobbleDuration);
  const float wobble = std::sin(instance.time * 42.f) * wobbleT * wobbleT * instance.hitWobbleStrength * 0.16f;

  const float flinchT = safeDivide(instance.flinchTimer, kFlinchDuration);
  const float flinch = flinchT * flinchT * instance.flinchStrength * -0.28f;

  const float spikeT = safeDivide(instance.pulseSpikeTimer, kPulseSpikeDuration);
  const float spike = spikeT * spikeT * instance.pulseSpikeStrength * 0.18f;

  for (size_t i = 0; i < boneCount; ++i) {
    const float t = boneCount == 1 ? 1.f : static_cast<float>(i) / static_cast<float>(boneCount - 1);
    instance.pose.local[i].rotation += wobble * (0.35f + t * 0.65f);
    if (i > 0) instance.pose.local[i].rotation += flinch * t;

    const float scale = 1.f + spike * (1.f - t * 0.35f);
    instance.pose.local[i].scale.x *= scale;
    instance.pose.local[i].scale.y *= scale;
  }
}

Transform2D reactionRootTransform(const AnimationInstance& instance) {
  Transform2D transform = instance.transform;
  transform.position.x += instance.recoilOffset.x;
  transform.position.y += instance.recoilOffset.y;
  return transform;
}

} // namespace

uint32_t addWaveChain(AnimationWorld& world, uint32_t instanceId, WaveChainParams params, float weight) {
  const uint32_t index = static_cast<uint32_t>(world.waveChains.size());
  world.waveChains.push_back({instanceId, params, weight});
  return index;
}

uint32_t addSpringChain(AnimationWorld& world, uint32_t instanceId, SpringChainParams params, float weight) {
  const uint32_t index = static_cast<uint32_t>(world.springChains.size());
  world.springChains.push_back({instanceId, params, weight});
  return index;
}

uint32_t addBendChain(AnimationWorld& world, uint32_t instanceId, BendChainParams params, float weight) {
  const uint32_t index = static_cast<uint32_t>(world.bendChains.size());
  world.bendChains.push_back({instanceId, params, weight});
  return index;
}

uint32_t addPulse(AnimationWorld& world, uint32_t instanceId, PulseParams params, float weight) {
  const uint32_t index = static_cast<uint32_t>(world.pulses.size());
  world.pulses.push_back({instanceId, params, weight});
  return index;
}

uint32_t addLookAt(AnimationWorld& world, uint32_t instanceId, LookAtParams params, float weight) {
  const uint32_t index = static_cast<uint32_t>(world.lookAts.size());
  world.lookAts.push_back({instanceId, params, weight});
  return index;
}

uint32_t addRotateToVelocity(AnimationWorld& world, uint32_t instanceId, RotateToVelocityParams params, float weight) {
  const uint32_t index = static_cast<uint32_t>(world.rotateToVelocity.size());
  world.rotateToVelocity.push_back({instanceId, params, weight});
  return index;
}

uint32_t addSpineBody(AnimationWorld& world, SpineBodyRenderParams params) {
  const uint32_t index = static_cast<uint32_t>(world.spineBodies.size());
  world.spineBodies.push_back(params);
  return index;
}

uint32_t addAppendage(AnimationWorld& world, AppendageRenderParams params) {
  const uint32_t index = static_cast<uint32_t>(world.appendages.size());
  world.appendages.push_back(params);
  return index;
}

uint32_t addCircleBody(AnimationWorld& world, CircleBodyRenderParams params) {
  const uint32_t index = static_cast<uint32_t>(world.circleBodies.size());
  world.circleBodies.push_back(params);
  return index;
}

uint32_t addAnimationArchetype(AnimationWorld& world, AnimationArchetype archetype) {
  const uint32_t index = static_cast<uint32_t>(world.archetypes.size());
  world.archetypes.push_back(archetype);
  return index;
}

void updateAnimationWorld(AnimationWorld& world, float dt) {
  for (AnimationInstance& instance : world.instances) {
    instance.time += dt;

    if (instance.skeletonId >= world.skeletons.size()) continue;
    resetToBindPose(world.skeletons[instance.skeletonId], instance.pose);
  }

  updateRotateToVelocity(world, dt);
  updateLookAts(world);
  updateBendChains(world);
  updateWaveChains(world);
  updatePulses(world);
  updateSpringChains(world, dt);

  for (AnimationInstance& instance : world.instances) {
    if (instance.skeletonId >= world.skeletons.size()) continue;
    applyReactionPose(instance);
    updateReactionTimers(instance, dt);
    solveWorldPose(world.skeletons[instance.skeletonId], reactionRootTransform(instance), instance.pose);
  }
}

void triggerRecoil(AnimationInstance& instance, Vector2 direction, float strength) {
  const Vector2 dir = normalizedOrZero(direction);
  const float clampedStrength = std::max(0.f, strength);
  instance.recoilVelocity.x += dir.x * clampedStrength * 260.f;
  instance.recoilVelocity.y += dir.y * clampedStrength * 260.f;
  instance.recoilTimer = kRecoilDuration;
  instance.recoilStrength = clampedStrength;
}

void triggerHitReaction(AnimationInstance& instance, float strength) {
  instance.hitWobbleTimer = kHitWobbleDuration;
  instance.hitWobbleStrength = std::max(0.f, strength);
}

void triggerPulseSpike(AnimationInstance& instance, float strength) {
  instance.pulseSpikeTimer = kPulseSpikeDuration;
  instance.pulseSpikeStrength = std::max(0.f, strength);
}

void triggerTentacleFlinch(AnimationInstance& instance, float strength) {
  instance.flinchTimer = kFlinchDuration;
  instance.flinchStrength = std::max(0.f, strength);
}

void triggerColorFlash(AnimationInstance& instance, float strength) {
  instance.colorFlashTimer = kColorFlashDuration;
  instance.colorFlashStrength = std::max(0.f, strength);
  instance.colorFlashAmount = std::clamp(instance.colorFlashStrength, 0.f, 1.f);
}

void triggerRecoil(AnimationWorld& world, uint32_t instanceId, Vector2 direction, float strength) {
  if (instanceId >= world.instances.size()) return;
  triggerRecoil(world.instances[instanceId], direction, strength);
}

void triggerHitReaction(AnimationWorld& world, uint32_t instanceId, float strength) {
  if (instanceId >= world.instances.size()) return;
  triggerHitReaction(world.instances[instanceId], strength);
}

void triggerPulseSpike(AnimationWorld& world, uint32_t instanceId, float strength) {
  if (instanceId >= world.instances.size()) return;
  triggerPulseSpike(world.instances[instanceId], strength);
}

void triggerTentacleFlinch(AnimationWorld& world, uint32_t instanceId, float strength) {
  if (instanceId >= world.instances.size()) return;
  triggerTentacleFlinch(world.instances[instanceId], strength);
}

void triggerColorFlash(AnimationWorld& world, uint32_t instanceId, float strength) {
  if (instanceId >= world.instances.size()) return;
  triggerColorFlash(world.instances[instanceId], strength);
}

Color applyColorFlash(Color color, float flashAmount) {
  const float t = std::clamp(flashAmount, 0.f, 1.f);
  return Color{
    static_cast<unsigned char>(static_cast<float>(color.r) + (255.f - static_cast<float>(color.r)) * t),
    static_cast<unsigned char>(static_cast<float>(color.g) + (255.f - static_cast<float>(color.g)) * t),
    static_cast<unsigned char>(static_cast<float>(color.b) + (255.f - static_cast<float>(color.b)) * t),
    color.a
  };
}

void updateRotateToVelocity(AnimationWorld& world, float dt) {
  for (const RotateToVelocityComponent& component : world.rotateToVelocity) {
    if (component.instanceId >= world.instances.size()) continue;

    AnimationInstance& instance = world.instances[component.instanceId];
    if (instance.skeletonId >= world.skeletons.size()) continue;

    applyRotateToVelocity(component.params, instance, dt, component.weight);
  }
}

void updateLookAts(AnimationWorld& world) {
  for (const LookAtComponent& component : world.lookAts) {
    if (component.instanceId >= world.instances.size()) continue;

    AnimationInstance& instance = world.instances[component.instanceId];
    if (instance.skeletonId >= world.skeletons.size()) continue;

    applyLookAt(world.skeletons[instance.skeletonId], component.params, instance, component.weight);
  }
}

void updateBendChains(AnimationWorld& world) {
  for (const BendChainComponent& component : world.bendChains) {
    if (component.instanceId >= world.instances.size()) continue;

    AnimationInstance& instance = world.instances[component.instanceId];
    if (instance.skeletonId >= world.skeletons.size()) continue;

    applyBendChain(component.params, instance, component.weight);
  }
}

void updateWaveChains(AnimationWorld& world) {
  for (const WaveChainComponent& component : world.waveChains) {
    if (component.instanceId >= world.instances.size()) continue;

    AnimationInstance& instance = world.instances[component.instanceId];
    if (instance.skeletonId >= world.skeletons.size()) continue;

    applyWaveChain(component.params, instance, component.weight);
  }
}

void updatePulses(AnimationWorld& world) {
  for (const PulseComponent& component : world.pulses) {
    if (component.instanceId >= world.instances.size()) continue;

    AnimationInstance& instance = world.instances[component.instanceId];
    if (instance.skeletonId >= world.skeletons.size()) continue;

    applyPulse(component.params, instance, component.weight);
  }
}

void updateSpringChains(AnimationWorld& world, float dt) {
  for (const SpringChainComponent& component : world.springChains) {
    if (component.instanceId >= world.instances.size()) continue;

    AnimationInstance& instance = world.instances[component.instanceId];
    if (instance.skeletonId >= world.skeletons.size()) continue;

    applySpringChain(world.skeletons[instance.skeletonId], component.params, instance, dt, component.weight);
  }
}

float lerp ( float a, float b, float t) {
  return a + (b - a) * t;
}

void applyWaveChain( const WaveChainParams& params,  AnimationInstance& instance, float weight ) {
  float speed = std::clamp(instance.moveAmount, 0.f, 1.f);

  float amplitude = lerp(params.idleAmplitude, params.moveAmplitude, speed);
  float frequency = lerp(params.idleFrequency, params.moveFrequency, speed);

  for ( uint16_t i = 0; i < params.bones.count; ++i) {
    uint16_t boneIndex = params.bones.start + i;

    float wave = std::sin(
      instance.time * frequency - static_cast<float>(i) * params.phaseOffset
    );

    instance.pose.local[boneIndex].rotation += wave * amplitude * weight;
  }
}

void applyBendChain( const BendChainParams& params, AnimationInstance& instance, float weight ) {
  if (params.bones.count == 0) return;

  const float turn = std::clamp(instance.turnAmount, -1.f, 1.f);

  for ( uint16_t i = 0; i < params.bones.count; ++i ) {
    const uint16_t boneIndex = params.bones.start + i;
    const float t = params.bones.count == 1
      ? 1.f
      : static_cast<float>(i) / static_cast<float>(params.bones.count - 1);
    const float distributed = std::pow(t, params.distribution);

    instance.pose.local[boneIndex].rotation += turn * params.maxAngle * distributed * weight;
  }
}

void applySpringChain( const Skeleton& skeleton, const SpringChainParams& params, AnimationInstance& instance, const float dt, float weight ) {
  if (params.bones.count == 0) return;

  const size_t boneCount = skeleton.bones.size();
  instance.springRotation.resize(boneCount, 0.f);
  instance.springVelocity.resize(boneCount, 0.f);

  for ( uint16_t i = 0; i < params.bones.count; ++i ) {
    const uint16_t boneIndex = params.bones.start + i;
    if (boneIndex >= boneCount) return;

    const float bindRotation = skeleton.bones[boneIndex].bindLocal.rotation;
    const float targetOffset = (instance.pose.local[boneIndex].rotation - bindRotation) * weight;
    const float displacement = targetOffset - instance.springRotation[boneIndex];
    const float acceleration = displacement * params.stiffness - instance.springVelocity[boneIndex] * params.damping;

    instance.springVelocity[boneIndex] += acceleration * dt;
    instance.springRotation[boneIndex] += instance.springVelocity[boneIndex] * dt;
    instance.pose.local[boneIndex].rotation = bindRotation + instance.springRotation[boneIndex];
  }
}

void applyPulse( const PulseParams& params, AnimationInstance& instance, float weight ) {
  for ( uint16_t i = 0; i < params.bones.count; ++i ) {
    const uint16_t boneIndex = params.bones.start + i;
    if (boneIndex >= instance.pose.local.size()) return;

    const float pulse = std::sin(instance.time * params.frequency - static_cast<float>(i) * params.phaseOffset);
    const float scale = 1.f + pulse * params.amplitude * weight;

    instance.pose.local[boneIndex].scale.y *= scale;
    instance.pose.local[boneIndex].scale.x *= 1.f - (scale - 1.f) * 0.25f;
  }
}

Transform2D combine(const Transform2D& parent, const Transform2D& child) {
  float c = std::cos(parent.rotation);
  float s = std::sin(parent.rotation);

  Vector2 rotated {
    child.position.x * c - child.position.y * s,
    child.position.x * s + child.position.y * c
  };

  Transform2D result;
  result.position = {
    parent.position.x + rotated.x,
    parent.position.y + rotated.y
  };

  result.rotation = parent.rotation + child.rotation;

  result.scale = {
    parent.scale.x * child.scale.x,
    parent.scale.y * child.scale.y
  };

  return result;
}

float angleDifference ( float target, float current ) {
  float diff = target - current;

  while (diff > PI) diff -= 2.f * PI;
  while (diff < -PI) diff += 2.f * PI;

  return diff;
}

void applyLookAt( const Skeleton& skeleton, const LookAtParams& params, AnimationInstance& instance, float weight) {
  if (params.bone >= skeleton.bones.size()) return;

  solveWorldPose(skeleton, instance.transform, instance.pose);

  const Transform2D& boneWorld = instance.pose.world[params.bone];
  const Vector2 target = instance.aimTarget.x != 0.f || instance.aimTarget.y != 0.f
    ? instance.aimTarget
    : params.target;
  const Vector2 diff = {
    target.x - boneWorld.position.x,
    target.y - boneWorld.position.y
  };

  if (diff.x == 0.f && diff.y == 0.f) return;

  const int parent = skeleton.bones[params.bone].parent;
  const float parentRotation = parent < 0
    ? instance.transform.rotation
    : instance.pose.world[parent].rotation;
  const float targetWorldRotation = std::atan2(diff.y, diff.x);
  const float targetLocalRotation = targetWorldRotation - parentRotation;
  const float diffLocal = angleDifference(targetLocalRotation, instance.pose.local[params.bone].rotation);

  instance.pose.local[params.bone].rotation += diffLocal * params.weight * weight;
}

void applyRotateToVelocity( const RotateToVelocityParams& params, AnimationInstance& instance, const float dt, float weight) {
  if ( instance.velocity.x == 0.f && instance.velocity.y == 0.f) return;

  float target = std::atan2(instance.velocity.y, instance.velocity.x);
  float diff = angleDifference(target, instance.transform.rotation);

  float t = params.turnSpeed * dt * weight;
  if ( t > 1.f) t = 1.f;

  instance.transform.rotation += diff * t;
}

void solveWorldPose( const Skeleton& skeleton, const Transform2D& rootTransform, Pose& pose ) {
  for (size_t i = 0; i < skeleton.bones.size(); ++i){
    int parent = skeleton.bones[i].parent;

    if (parent < 0) {
      pose.world[i] = combine(rootTransform, pose.local[i]);
    } else {
      pose.world[i] = combine(pose.world[parent], pose.local[i]);
    }
  }
}

void resetToBindPose(const Skeleton& skeleton, Pose& pose) {
  size_t count = skeleton.bones.size();

  pose.local.resize(count);
  pose.world.resize(count);

  for ( size_t i = 0; i < count; ++i ) {
    pose.local[i] = skeleton.bones[i].bindLocal;
  }
}

// ---- FISH ---- //

void createFish(AnimationWorld& world) {
  const uint32_t skeletonID = static_cast<uint32_t>(world.skeletons.size());
  world.skeletons.push_back(createFishSkeleton());

  AnimationInstance fish;
  fish.skeletonId = skeletonID;
  fish.moveAmount = 1.0f;
  fish.transform = Transform2D{{GetScreenWidth() / 2.f, GetScreenHeight() / 2.f}, 1.0, {1.f,1.f}};
  fish.velocity = {-6.f, 0.f};

  const uint32_t instanceId = static_cast<uint32_t>(world.instances.size());
  world.instances.push_back(fish);

  WaveChainParams wave;
  wave.bones = {1, 4};
  wave.idleAmplitude = 0.15f;
  wave.idleFrequency = 4.0f;
  wave.moveAmplitude = 0.35f;
  wave.moveFrequency = 8.f;
  wave.phaseOffset = 0.8f;

  RotateToVelocityParams rotateToVelocity = {5.f};

  AnimationArchetype archetype;
  archetype.name = "Fish";
  archetype.skeletonId = skeletonID;
  archetype.rotateToVelocityIds.push_back(addRotateToVelocity(world, instanceId, rotateToVelocity));
  archetype.waveChainIds.push_back(addWaveChain(world, instanceId, wave));
  addAnimationArchetype(world, archetype);
}

void createSpineShip(AnimationWorld& world) {
  const uint32_t skeletonID = static_cast<uint32_t>(world.skeletons.size());
  world.skeletons.push_back(createSpineShipSkeleton());

  SpineBodyRenderParams body;
  body.bones = {0, 6};
  body.color = Color{60, 210, 190, 220};
  body.outlineColor = Color{190, 255, 240, 255};
  body.widths = {5.f, 10.f, 22.f, 26.f, 15.f, 4.f};
  const uint32_t bodyIndex = addSpineBody(world, body);

  AnimationInstance ship;
  ship.skeletonId = skeletonID;
  ship.hasSpineBody = true;
  ship.spineBodyId = bodyIndex;
  ship.moveAmount = 1.0f;
  ship.transform = Transform2D{{GetScreenWidth() / 2.f + 120.f, GetScreenHeight() / 2.f}, 0.f, {1.f,1.f}};
  ship.velocity = {-6.f, 0.f};

  const uint32_t instanceId = static_cast<uint32_t>(world.instances.size());
  world.instances.push_back(ship);

  BendChainParams bend;
  bend.bones = {1, 5};
  bend.maxAngle = 0.35f;
  bend.distribution = 1.25f;

  WaveChainParams wave;
  wave.bones = {2, 4};
  wave.idleAmplitude = 0.025f;
  wave.idleFrequency = 2.0f;
  wave.moveAmplitude = 0.08f;
  wave.moveFrequency = 5.0f;
  wave.phaseOffset = 0.65f;

  RotateToVelocityParams rotateToVelocity = {4.f};

  SpringChainParams spring;
  spring.bones = {1, 5};
  spring.stiffness = 70.f;
  spring.damping = 12.f;

  PulseParams pulse;
  pulse.bones = {1, 4};
  pulse.amplitude = 0.08f;
  pulse.frequency = 3.0f;
  pulse.phaseOffset = 0.7f;

  LookAtParams lookAt;
  lookAt.bone = 6;
  lookAt.weight = 0.85f;

  AnimationArchetype archetype;
  archetype.name = "Spine Ship";
  archetype.skeletonId = skeletonID;
  archetype.hasSpineBody = true;
  archetype.spineBodyId = bodyIndex;
  archetype.rotateToVelocityIds.push_back(addRotateToVelocity(world, instanceId, rotateToVelocity));
  archetype.lookAtIds.push_back(addLookAt(world, instanceId, lookAt));
  archetype.bendChainIds.push_back(addBendChain(world, instanceId, bend));
  archetype.waveChainIds.push_back(addWaveChain(world, instanceId, wave));
  archetype.pulseIds.push_back(addPulse(world, instanceId, pulse));
  archetype.springChainIds.push_back(addSpringChain(world, instanceId, spring));
  addAnimationArchetype(world, archetype);
}

void createMantaShip(AnimationWorld& world) {
  const uint32_t skeletonID = static_cast<uint32_t>(world.skeletons.size());
  world.skeletons.push_back(createMantaShipSkeleton());

  SpineBodyRenderParams body;
  body.bones = {0, 6};
  body.color = Color{120, 80, 230, 220};
  body.outlineColor = Color{225, 205, 255, 255};
  body.widths = {6.f, 16.f, 30.f, 34.f, 20.f, 5.f};
  const uint32_t bodyIndex = addSpineBody(world, body);

  AppendageRenderParams leftWing;
  leftWing.bones = {7, 3};
  leftWing.color = Color{90, 45, 190, 185};
  leftWing.outlineColor = Color{210, 190, 255, 230};
  leftWing.widths = {20.f, 34.f, 4.f};
  const uint32_t leftWingIndex = addAppendage(world, leftWing);

  AppendageRenderParams rightWing = leftWing;
  rightWing.bones = {10, 3};
  const uint32_t rightWingIndex = addAppendage(world, rightWing);

  AnimationInstance manta;
  manta.skeletonId = skeletonID;
  manta.hasSpineBody = true;
  manta.spineBodyId = bodyIndex;
  manta.appendageIds.push_back(leftWingIndex);
  manta.appendageIds.push_back(rightWingIndex);
  manta.moveAmount = 1.0f;
  manta.transform = Transform2D{{GetScreenWidth() / 2.f - 160.f, GetScreenHeight() / 2.f + 120.f}, 0.f, {1.f,1.f}};
  manta.velocity = {-6.f, 0.f};

  const uint32_t instanceId = static_cast<uint32_t>(world.instances.size());
  world.instances.push_back(manta);

  BendChainParams bend;
  bend.bones = {1, 5};
  bend.maxAngle = 0.22f;
  bend.distribution = 1.1f;

  WaveChainParams bodyWave;
  bodyWave.bones = {2, 4};
  bodyWave.idleAmplitude = 0.02f;
  bodyWave.idleFrequency = 1.5f;
  bodyWave.moveAmplitude = 0.06f;
  bodyWave.moveFrequency = 4.0f;
  bodyWave.phaseOffset = 0.55f;

  WaveChainParams leftWingWave;
  leftWingWave.bones = {7, 3};
  leftWingWave.idleAmplitude = 0.04f;
  leftWingWave.idleFrequency = 2.0f;
  leftWingWave.moveAmplitude = 0.14f;
  leftWingWave.moveFrequency = 5.5f;
  leftWingWave.phaseOffset = 0.8f;

  WaveChainParams rightWingWave = leftWingWave;
  rightWingWave.bones = {10, 3};

  RotateToVelocityParams rotateToVelocity = {1.5f};

  SpringChainParams bodySpring;
  bodySpring.bones = {1, 5};
  bodySpring.stiffness = 60.f;
  bodySpring.damping = 11.f;

  SpringChainParams leftWingSpring;
  leftWingSpring.bones = {7, 3};
  leftWingSpring.stiffness = 45.f;
  leftWingSpring.damping = 9.f;

  SpringChainParams rightWingSpring = leftWingSpring;
  rightWingSpring.bones = {10, 3};

  PulseParams pulse;
  pulse.bones = {1, 5};
  pulse.amplitude = 0.06f;
  pulse.frequency = 2.4f;
  pulse.phaseOffset = 0.55f;

  LookAtParams lookAt;
  lookAt.bone = 6;
  lookAt.weight = 0.75f;

  AnimationArchetype archetype;
  archetype.name = "Manta Ship";
  archetype.skeletonId = skeletonID;
  archetype.hasSpineBody = true;
  archetype.spineBodyId = bodyIndex;
  archetype.appendageIds.push_back(leftWingIndex);
  archetype.appendageIds.push_back(rightWingIndex);
  archetype.rotateToVelocityIds.push_back(addRotateToVelocity(world, instanceId, rotateToVelocity));
  archetype.lookAtIds.push_back(addLookAt(world, instanceId, lookAt));
  archetype.bendChainIds.push_back(addBendChain(world, instanceId, bend));
  archetype.waveChainIds.push_back(addWaveChain(world, instanceId, bodyWave));
  archetype.waveChainIds.push_back(addWaveChain(world, instanceId, leftWingWave));
  archetype.waveChainIds.push_back(addWaveChain(world, instanceId, rightWingWave, -1.f));
  archetype.pulseIds.push_back(addPulse(world, instanceId, pulse));
  archetype.springChainIds.push_back(addSpringChain(world, instanceId, bodySpring));
  archetype.springChainIds.push_back(addSpringChain(world, instanceId, leftWingSpring));
  archetype.springChainIds.push_back(addSpringChain(world, instanceId, rightWingSpring));
  addAnimationArchetype(world, archetype);
}

void createSerpentShip(AnimationWorld& world) {
  const uint32_t skeletonID = static_cast<uint32_t>(world.skeletons.size());
  world.skeletons.push_back(createSerpentShipSkeleton());

  SpineBodyRenderParams body;
  body.bones = {0, 10};
  body.color = Color{230, 110, 80, 215};
  body.outlineColor = Color{255, 215, 190, 255};
  body.widths = {5.f, 9.f, 15.f, 18.f, 17.f, 14.f, 11.f, 8.f, 5.f, 2.f};
  const uint32_t bodyIndex = addSpineBody(world, body);

  AnimationInstance serpent;
  serpent.skeletonId = skeletonID;
  serpent.hasSpineBody = true;
  serpent.spineBodyId = bodyIndex;
  serpent.moveAmount = 1.0f;
  serpent.transform = Transform2D{{GetScreenWidth() / 2.f + 220.f, GetScreenHeight() / 2.f + 170.f}, 0.f, {1.f,1.f}};
  serpent.velocity = {-6.f, 0.f};

  const uint32_t instanceId = static_cast<uint32_t>(world.instances.size());
  world.instances.push_back(serpent);

  BendChainParams bend;
  bend.bones = {1, 9};
  bend.maxAngle = 0.18f;
  bend.distribution = 1.4f;

  WaveChainParams wave;
  wave.bones = {2, 8};
  wave.idleAmplitude = 0.08f;
  wave.idleFrequency = 2.2f;
  wave.moveAmplitude = 0.22f;
  wave.moveFrequency = 5.8f;
  wave.phaseOffset = 0.9f;

  RotateToVelocityParams rotateToVelocity = {1.8f};

  SpringChainParams spring;
  spring.bones = {1, 9};
  spring.stiffness = 42.f;
  spring.damping = 8.f;

  PulseParams pulse;
  pulse.bones = {1, 8};
  pulse.amplitude = 0.035f;
  pulse.frequency = 2.8f;
  pulse.phaseOffset = 0.45f;

  LookAtParams lookAt;
  lookAt.bone = 10;
  lookAt.weight = 0.7f;

  AnimationArchetype archetype;
  archetype.name = "Serpent Ship";
  archetype.skeletonId = skeletonID;
  archetype.hasSpineBody = true;
  archetype.spineBodyId = bodyIndex;
  archetype.rotateToVelocityIds.push_back(addRotateToVelocity(world, instanceId, rotateToVelocity));
  archetype.lookAtIds.push_back(addLookAt(world, instanceId, lookAt));
  archetype.bendChainIds.push_back(addBendChain(world, instanceId, bend));
  archetype.waveChainIds.push_back(addWaveChain(world, instanceId, wave));
  archetype.pulseIds.push_back(addPulse(world, instanceId, pulse));
  archetype.springChainIds.push_back(addSpringChain(world, instanceId, spring));
  addAnimationArchetype(world, archetype);
}

void createEngineCreature(AnimationWorld& world) {
  const uint32_t skeletonID = static_cast<uint32_t>(world.skeletons.size());
  world.skeletons.push_back(createEngineCreatureSkeleton());

  SpineBodyRenderParams body;
  body.bones = {0, 5};
  body.color = Color{70, 220, 120, 215};
  body.outlineColor = Color{205, 255, 210, 255};
  body.widths = {7.f, 19.f, 28.f, 22.f, 8.f};
  const uint32_t bodyIndex = addSpineBody(world, body);

  AppendageRenderParams centerTendril;
  centerTendril.bones = {6, 3};
  centerTendril.color = Color{70, 255, 150, 170};
  centerTendril.outlineColor = Color{190, 255, 210, 220};
  centerTendril.widths = {8.f, 5.f, 2.f};
  const uint32_t centerTendrilIndex = addAppendage(world, centerTendril);

  AppendageRenderParams leftTendril = centerTendril;
  leftTendril.bones = {9, 3};
  const uint32_t leftTendrilIndex = addAppendage(world, leftTendril);

  AppendageRenderParams rightTendril = centerTendril;
  rightTendril.bones = {12, 3};
  const uint32_t rightTendrilIndex = addAppendage(world, rightTendril);

  AnimationInstance engine;
  engine.skeletonId = skeletonID;
  engine.hasSpineBody = true;
  engine.spineBodyId = bodyIndex;
  engine.appendageIds.push_back(leftTendrilIndex);
  engine.appendageIds.push_back(centerTendrilIndex);
  engine.appendageIds.push_back(rightTendrilIndex);
  engine.moveAmount = 1.0f;
  engine.transform = Transform2D{{GetScreenWidth() / 2.f - 240.f, GetScreenHeight() / 2.f - 160.f}, 0.f, {1.f,1.f}};
  engine.velocity = {-6.f, 0.f};

  const uint32_t instanceId = static_cast<uint32_t>(world.instances.size());
  world.instances.push_back(engine);

  BendChainParams bend;
  bend.bones = {1, 4};
  bend.maxAngle = 0.24f;
  bend.distribution = 1.2f;

  WaveChainParams centerTendrilWave;
  centerTendrilWave.bones = {6, 3};
  centerTendrilWave.idleAmplitude = 0.12f;
  centerTendrilWave.idleFrequency = 2.5f;
  centerTendrilWave.moveAmplitude = 0.34f;
  centerTendrilWave.moveFrequency = 7.0f;
  centerTendrilWave.phaseOffset = 0.85f;

  WaveChainParams leftTendrilWave = centerTendrilWave;
  leftTendrilWave.bones = {9, 3};
  leftTendrilWave.phaseOffset = 1.05f;

  WaveChainParams rightTendrilWave = centerTendrilWave;
  rightTendrilWave.bones = {12, 3};
  rightTendrilWave.phaseOffset = 1.05f;

  RotateToVelocityParams rotateToVelocity = {2.8f};

  SpringChainParams bodySpring;
  bodySpring.bones = {1, 4};
  bodySpring.stiffness = 55.f;
  bodySpring.damping = 10.f;

  SpringChainParams centerTendrilSpring;
  centerTendrilSpring.bones = {6, 3};
  centerTendrilSpring.stiffness = 38.f;
  centerTendrilSpring.damping = 7.5f;

  SpringChainParams leftTendrilSpring = centerTendrilSpring;
  leftTendrilSpring.bones = {9, 3};

  SpringChainParams rightTendrilSpring = centerTendrilSpring;
  rightTendrilSpring.bones = {12, 3};

  PulseParams pulse;
  pulse.bones = {1, 4};
  pulse.amplitude = 0.12f;
  pulse.frequency = 4.0f;
  pulse.phaseOffset = 0.35f;

  LookAtParams lookAt;
  lookAt.bone = 5;
  lookAt.weight = 0.8f;

  AnimationArchetype archetype;
  archetype.name = "Engine Creature";
  archetype.skeletonId = skeletonID;
  archetype.hasSpineBody = true;
  archetype.spineBodyId = bodyIndex;
  archetype.appendageIds.push_back(leftTendrilIndex);
  archetype.appendageIds.push_back(centerTendrilIndex);
  archetype.appendageIds.push_back(rightTendrilIndex);
  archetype.rotateToVelocityIds.push_back(addRotateToVelocity(world, instanceId, rotateToVelocity));
  archetype.lookAtIds.push_back(addLookAt(world, instanceId, lookAt));
  archetype.bendChainIds.push_back(addBendChain(world, instanceId, bend));
  archetype.pulseIds.push_back(addPulse(world, instanceId, pulse));
  archetype.waveChainIds.push_back(addWaveChain(world, instanceId, centerTendrilWave));
  archetype.waveChainIds.push_back(addWaveChain(world, instanceId, leftTendrilWave));
  archetype.waveChainIds.push_back(addWaveChain(world, instanceId, rightTendrilWave, -1.f));
  archetype.springChainIds.push_back(addSpringChain(world, instanceId, bodySpring));
  archetype.springChainIds.push_back(addSpringChain(world, instanceId, centerTendrilSpring));
  archetype.springChainIds.push_back(addSpringChain(world, instanceId, leftTendrilSpring));
  archetype.springChainIds.push_back(addSpringChain(world, instanceId, rightTendrilSpring));
  addAnimationArchetype(world, archetype);
}

void createOrbTentacleCreature(AnimationWorld& world) {
  const uint32_t skeletonID = static_cast<uint32_t>(world.skeletons.size());
  world.skeletons.push_back(createOrbTentacleCreatureSkeleton());

  CircleBodyRenderParams body;
  body.bone = 0;
  body.radius = 28.f;
  body.color = Color{210, 70, 240, 220};
  body.outlineColor = Color{255, 205, 255, 255};
  const uint32_t bodyIndex = addCircleBody(world, body);

  AnimationInstance orb;
  orb.skeletonId = skeletonID;
  orb.hasCircleBody = true;
  orb.circleBodyId = bodyIndex;
  orb.moveAmount = 1.0f;
  orb.transform = Transform2D{{GetScreenWidth() / 2.f + 260.f, GetScreenHeight() / 2.f - 170.f}, 0.f, {1.f,1.f}};
  orb.velocity = {-6.f, 0.f};

  AnimationArchetype archetype;
  archetype.name = "Orb Tentacle Creature";
  archetype.skeletonId = skeletonID;
  archetype.hasCircleBody = true;
  archetype.circleBodyId = bodyIndex;

  const uint16_t tentacleStarts[] = {2, 5, 8, 11, 14, 17};
  for (size_t i = 0; i < 6; ++i) {
    AppendageRenderParams tentacle;
    tentacle.bones = {tentacleStarts[i], 3};
    tentacle.color = Color{205, 80, 255, 150};
    tentacle.outlineColor = Color{250, 190, 255, 220};
    tentacle.widths = {9.f, 5.f, 2.f};
    const uint32_t appendageIndex = addAppendage(world, tentacle);

    orb.appendageIds.push_back(appendageIndex);
    archetype.appendageIds.push_back(appendageIndex);
  }

  const uint32_t instanceId = static_cast<uint32_t>(world.instances.size());
  world.instances.push_back(orb);

  RotateToVelocityParams rotateToVelocity = {2.2f};

  PulseParams pulse;
  pulse.bones = {0, 1};
  pulse.amplitude = 0.16f;
  pulse.frequency = 3.5f;
  pulse.phaseOffset = 0.f;

  LookAtParams lookAt;
  lookAt.bone = 1;
  lookAt.weight = 0.65f;

  archetype.rotateToVelocityIds.push_back(addRotateToVelocity(world, instanceId, rotateToVelocity));
  archetype.lookAtIds.push_back(addLookAt(world, instanceId, lookAt));
  archetype.pulseIds.push_back(addPulse(world, instanceId, pulse));

  const float waveWeights[] = {1.f, -1.f, 0.85f, -0.85f, 1.15f, -1.15f};
  for (size_t i = 0; i < 6; ++i) {
    WaveChainParams wave;
    wave.bones = {tentacleStarts[i], 3};
    wave.idleAmplitude = 0.10f;
    wave.idleFrequency = 1.8f + static_cast<float>(i) * 0.12f;
    wave.moveAmplitude = 0.34f;
    wave.moveFrequency = 5.8f + static_cast<float>(i) * 0.25f;
    wave.phaseOffset = 0.95f;

    SpringChainParams spring;
    spring.bones = {tentacleStarts[i], 3};
    spring.stiffness = 34.f;
    spring.damping = 7.f;

    archetype.waveChainIds.push_back(addWaveChain(world, instanceId, wave, waveWeights[i]));
    archetype.springChainIds.push_back(addSpringChain(world, instanceId, spring));
  }

  addAnimationArchetype(world, archetype);
}

Skeleton createFishSkeleton() {
  Skeleton s;

  //root
  s.bones.push_back(Bone{-1, Transform2D{{0,0}, 0.f, {1,1}}});

  //spines
  s.bones.push_back(Bone{0, Transform2D{{-20,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{1, Transform2D{{-20,0}, 0.f, {1.f,1.f}}});

  //tails
  s.bones.push_back(Bone{2, Transform2D{{-20,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{3, Transform2D{{-15,0}, 0.f, {1.f,1.f}}});

  return s;
}

Skeleton createSpineShipSkeleton() {
  Skeleton s;

  s.bones.push_back(Bone{-1, Transform2D{{0,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{0, Transform2D{{18,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{1, Transform2D{{-22,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{2, Transform2D{{-24,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{3, Transform2D{{-22,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{4, Transform2D{{-16,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{1, Transform2D{{14,0}, 0.f, {1.f,1.f}}});

  return s;
}

Skeleton createMantaShipSkeleton() {
  Skeleton s;

  // Body spine: root, nose, body, belly, rear, tail.
  s.bones.push_back(Bone{-1, Transform2D{{0,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{0, Transform2D{{22,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{1, Transform2D{{-24,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{2, Transform2D{{-24,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{3, Transform2D{{-22,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{4, Transform2D{{-16,0}, 0.f, {1.f,1.f}}});

  // Sensor stalk.
  s.bones.push_back(Bone{1, Transform2D{{15,0}, 0.f, {1.f,1.f}}});

  // Left wing chain.
  s.bones.push_back(Bone{2, Transform2D{{-4,-18}, -0.35f, {1.f,1.f}}});
  s.bones.push_back(Bone{7, Transform2D{{-10,-34}, -0.25f, {1.f,1.f}}});
  s.bones.push_back(Bone{8, Transform2D{{-6,-34}, -0.15f, {1.f,1.f}}});

  // Right wing chain.
  s.bones.push_back(Bone{2, Transform2D{{-4,18}, 0.35f, {1.f,1.f}}});
  s.bones.push_back(Bone{10, Transform2D{{-10,34}, 0.25f, {1.f,1.f}}});
  s.bones.push_back(Bone{11, Transform2D{{-6,34}, 0.15f, {1.f,1.f}}});

  return s;
}

Skeleton createSerpentShipSkeleton() {
  Skeleton s;

  s.bones.push_back(Bone{-1, Transform2D{{0,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{0, Transform2D{{18,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{1, Transform2D{{-20,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{2, Transform2D{{-20,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{3, Transform2D{{-19,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{4, Transform2D{{-18,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{5, Transform2D{{-17,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{6, Transform2D{{-16,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{7, Transform2D{{-15,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{8, Transform2D{{-12,0}, 0.f, {1.f,1.f}}});

  // Head sensor/mandible bone, outside the rendered body range.
  s.bones.push_back(Bone{1, Transform2D{{12,0}, 0.f, {1.f,1.f}}});

  return s;
}

Skeleton createEngineCreatureSkeleton() {
  Skeleton s;

  // Compact core body.
  s.bones.push_back(Bone{-1, Transform2D{{0,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{0, Transform2D{{20,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{1, Transform2D{{-22,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{2, Transform2D{{-22,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{3, Transform2D{{-14,0}, 0.f, {1.f,1.f}}});

  // Forward sensor.
  s.bones.push_back(Bone{1, Transform2D{{13,0}, 0.f, {1.f,1.f}}});

  // Center rear tendril.
  s.bones.push_back(Bone{4, Transform2D{{-10,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{6, Transform2D{{-15,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{7, Transform2D{{-13,0}, 0.f, {1.f,1.f}}});

  // Left rear tendril.
  s.bones.push_back(Bone{4, Transform2D{{-8,-10}, -0.35f, {1.f,1.f}}});
  s.bones.push_back(Bone{9, Transform2D{{-14,-5}, -0.2f, {1.f,1.f}}});
  s.bones.push_back(Bone{10, Transform2D{{-12,-3}, -0.1f, {1.f,1.f}}});

  // Right rear tendril.
  s.bones.push_back(Bone{4, Transform2D{{-8,10}, 0.35f, {1.f,1.f}}});
  s.bones.push_back(Bone{12, Transform2D{{-14,5}, 0.2f, {1.f,1.f}}});
  s.bones.push_back(Bone{13, Transform2D{{-12,3}, 0.1f, {1.f,1.f}}});

  return s;
}

Skeleton createOrbTentacleCreatureSkeleton() {
  Skeleton s;

  // Core and short forward sensor.
  s.bones.push_back(Bone{-1, Transform2D{{0,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{0, Transform2D{{24,0}, 0.f, {1.f,1.f}}});

  // Six outgoing tentacles around the orb.
  s.bones.push_back(Bone{0, Transform2D{{27,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{2, Transform2D{{30,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{3, Transform2D{{28,0}, 0.f, {1.f,1.f}}});

  s.bones.push_back(Bone{0, Transform2D{{14,-24}, -1.05f, {1.f,1.f}}});
  s.bones.push_back(Bone{5, Transform2D{{30,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{6, Transform2D{{28,0}, 0.f, {1.f,1.f}}});

  s.bones.push_back(Bone{0, Transform2D{{-14,-24}, -2.1f, {1.f,1.f}}});
  s.bones.push_back(Bone{8, Transform2D{{30,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{9, Transform2D{{28,0}, 0.f, {1.f,1.f}}});

  s.bones.push_back(Bone{0, Transform2D{{-27,0}, PI, {1.f,1.f}}});
  s.bones.push_back(Bone{11, Transform2D{{30,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{12, Transform2D{{28,0}, 0.f, {1.f,1.f}}});

  s.bones.push_back(Bone{0, Transform2D{{-14,24}, 2.1f, {1.f,1.f}}});
  s.bones.push_back(Bone{14, Transform2D{{30,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{15, Transform2D{{28,0}, 0.f, {1.f,1.f}}});

  s.bones.push_back(Bone{0, Transform2D{{14,24}, 1.05f, {1.f,1.f}}});
  s.bones.push_back(Bone{17, Transform2D{{30,0}, 0.f, {1.f,1.f}}});
  s.bones.push_back(Bone{18, Transform2D{{28,0}, 0.f, {1.f,1.f}}});

  return s;
}

// ---- DEBUG DRAW --- //

float widthAt(const std::vector<float>& widths, size_t index) {
  if (widths.empty()) return 8.f;
  if (index >= widths.size()) return widths.back();
  return widths[index];
}

float widthAt(const SpineBodyRenderParams& params, size_t index) {
  return widthAt(params.widths, index);
}

float widthAt(const AppendageRenderParams& params, size_t index) {
  if (params.widths.empty()) return 8.f;
  if (index >= params.widths.size()) return params.widths.back();
  return params.widths[index];
}

void drawAppendage(const AppendageRenderParams& params, const Pose& pose, float flashAmount) {
  if (params.bones.count < 2) return;

  const Color color = applyColorFlash(params.color, flashAmount);
  const Color outlineColor = applyColorFlash(params.outlineColor, flashAmount * 0.7f);

  std::vector<Vector2> left;
  std::vector<Vector2> right;
  left.reserve(params.bones.count);
  right.reserve(params.bones.count);

  for (uint16_t i = 0; i < params.bones.count; ++i) {
    const uint16_t boneIndex = params.bones.start + i;
    if (boneIndex >= pose.world.size()) return;

    const Transform2D& t = pose.world[boneIndex];
    const Vector2 normal = { -std::sin(t.rotation), std::cos(t.rotation) };
    const float width = widthAt(params, i) * t.scale.y;

    left.push_back({ t.position.x + normal.x * width, t.position.y + normal.y * width });
    right.push_back({ t.position.x - normal.x * width, t.position.y - normal.y * width });
  }

  for (size_t i = 0; i + 1 < left.size(); ++i) {
    DrawTriangle(left[i], left[i + 1], right[i], color);
    DrawTriangle(right[i], left[i + 1], right[i + 1], color);

    DrawLineEx(left[i], left[i + 1], 2.f, outlineColor);
    DrawLineEx(right[i], right[i + 1], 2.f, outlineColor);
  }

  DrawLineEx(left.front(), right.front(), 2.f, outlineColor);
  DrawLineEx(left.back(), right.back(), 2.f, outlineColor);
}

void drawCircleBody(const CircleBodyRenderParams& params, const Pose& pose, float flashAmount) {
  if (params.bone >= pose.world.size()) return;

  const Transform2D& t = pose.world[params.bone];
  const float scale = (t.scale.x + t.scale.y) * 0.5f;
  const float radius = params.radius * scale;
  const Color color = applyColorFlash(params.color, flashAmount);
  const Color outlineColor = applyColorFlash(params.outlineColor, flashAmount * 0.7f);

  DrawCircleV(t.position, radius, color);
  DrawCircleLines(static_cast<int>(t.position.x), static_cast<int>(t.position.y), radius, outlineColor);
}

void drawSpineBody(const SpineBodyRenderParams& params, const Pose& pose, float flashAmount) {
  if (params.bones.count < 2) return;

  const Color color = applyColorFlash(params.color, flashAmount);
  const Color outlineColor = applyColorFlash(params.outlineColor, flashAmount * 0.7f);

  std::vector<Vector2> left;
  std::vector<Vector2> right;
  left.reserve(params.bones.count);
  right.reserve(params.bones.count);

  for (uint16_t i = 0; i < params.bones.count; ++i) {
    const uint16_t boneIndex = params.bones.start + i;
    if (boneIndex >= pose.world.size()) return;

    const Transform2D& t = pose.world[boneIndex];
    const Vector2 normal = { -std::sin(t.rotation), std::cos(t.rotation) };
    const float width = widthAt(params, i) * t.scale.y;

    left.push_back({ t.position.x + normal.x * width, t.position.y + normal.y * width });
    right.push_back({ t.position.x - normal.x * width, t.position.y - normal.y * width });
  }

  for (size_t i = 0; i + 1 < left.size(); ++i) {
    DrawTriangle(left[i], left[i + 1], right[i], color);
    DrawTriangle(right[i], left[i + 1], right[i + 1], color);

    DrawLineEx(left[i], left[i + 1], 2.f, outlineColor);
    DrawLineEx(right[i], right[i + 1], 2.f, outlineColor);
  }

  DrawLineEx(left.front(), right.front(), 2.f, outlineColor);
  DrawLineEx(left.back(), right.back(), 2.f, outlineColor);
}

void drawSkeleton(const Skeleton& skeleton, const Pose& pose) {
  for (size_t i = 0; i < skeleton.bones.size(); ++i) {
    const Vector2 p = pose.world[i].position;

    //joint
    DrawCircleV(p, 4.0, GRAY);

    int parent = skeleton.bones[i].parent;
    if (parent >= 0) {
      Vector2 pp = pose.world[parent].position;

      //bone line
      DrawLineEx(pp, p, 2.f, GREEN);
    }

    bool hasChild = false;
    for (const Bone& bone : skeleton.bones) {
      if (bone.parent == static_cast<int>(i)) {
        hasChild = true;
        break;
      }
    }

    if (!hasChild) {
      Vector2 dir = { std::cos(pose.world[i].rotation), std::sin(pose.world[i].rotation) };
      Vector2 end = { p.x + dir.x * 18.f, p.y + dir.y * 18.f };
      DrawLineEx(p, end, 2.f, SKYBLUE);
    }
  }
}

void drawBoneDirection(const Pose& pose){
  for (const Transform2D& t: pose.world) {
    Vector2 p = t.position;
    Vector2 dir = { std::cos(t.rotation), std::sin(t.rotation) };
    Vector2 end = { p.x + dir.x * 15.f, p.y + dir.y * 15.f };

    DrawLineEx(p, end, 2.f, RED);
  }
}

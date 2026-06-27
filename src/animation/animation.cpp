#include "animation.h"
#include "raylib.h"
#include <cmath>
#include <cstdint>
#include <vector>


// main driver of animations
void updateAnimation( AnimationDatabase& db, AnimationInstance& instance, float dt) {
  instance.time += dt;

  const Skeleton& skeleton = db.skeletons[instance.skeletonId];

  resetToBindPose(skeleton, instance.pose);

  for ( const Procedure& proc : instance.procedures ) {
    switch (proc.type) {
      case ProcedureType::WaveChain:
        applyWaveChain(db.waveChains[proc.paramIndex], instance);
       break;
      case ProcedureType::LookAt:
       break;
      case ProcedureType::Spring:
       break;
      case ProcedureType::RotateToVelocity:
        applyRotateToVelocity(db.rotateToVelocity[proc.paramIndex], instance, dt);
       break;
    }
  }

  solveWorldPose(skeleton, instance.transform, instance.pose);

}

float lerp ( float a, float b, float t) {
  return a + (b - a) * t;
}

void applyWaveChain( const WaveChainParams& params,  AnimationInstance& instance ) {
  float speed = instance.speed;

  if (speed < 0.f) speed = 0.f;
  if (speed > 1.f) speed = 1.f;

  float amplitude = lerp(params.idleAmplitude, params.moveAmplitude, speed);
  float frequency = lerp(params.idleFrequency, params.moveFrequency, speed);

  for ( uint16_t i = 0; i < params.bones.count; ++i) {
    uint16_t boneIndex = params.bones.start + i;

    float wave = std::sin(
      instance.time * frequency - static_cast<float>(i) * params.phaseOffset
    );

    instance.pose.local[boneIndex].rotation += wave * amplitude;
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

void applyRotateToVelocity( const RotateToVelocityParams& params, AnimationInstance& instance, const float dt) {
  if ( instance.velocity.x == 0.f && instance.velocity.y == 0.f) return;

  float target = std::atan2(instance.velocity.y, instance.velocity.x);
  float diff = angleDifference(target, instance.transform.rotation);

  float t = params.turnSpeed * dt;
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

void createFish(AnimationDatabase& db, std::vector<AnimationInstance>& instances) {
  uint32_t skeletonID = db.skeletons.size();
  
  db.skeletons.push_back(createFishSkeleton());

  WaveChainParams wave;
  wave.bones = {1, 4};
  wave.idleAmplitude = 0.15f;
  wave.idleFrequency = 4.0f;

  wave.moveAmplitude = 0.35f;
  wave.moveFrequency = 8.f;

  wave.phaseOffset = 0.8f;
  uint32_t waveIndex = db.waveChains.size();
  db.waveChains.push_back(wave);

  RotateToVelocityParams rotateToVelocity = {5.f};
  uint32_t rotateToVelocityIndex = db.rotateToVelocity.size();
  db.rotateToVelocity.push_back(rotateToVelocity);

  AnimationInstance fish;
  fish.skeletonId = skeletonID;
  fish.speed = 1.0f;
  fish.transform = Transform2D{{GetScreenWidth() / 2.f, GetScreenHeight() / 2.f}, 1.0, {1.f,1.f}};
  fish.velocity = {-6.f, 0.f};

  // Order matters
  fish.procedures.push_back({ProcedureType::RotateToVelocity, rotateToVelocityIndex});
  fish.procedures.push_back({ProcedureType::WaveChain, waveIndex});

  instances.push_back(fish);
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

// ---- DEBUG DRAW --- //
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

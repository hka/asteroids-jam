void createEditedRig(AnimationWorld& world) {
  const uint32_t skeletonId = static_cast<uint32_t>(world.skeletons.size());
  Skeleton skeleton;
  skeleton.bones.push_back(Bone{-1, Transform2D{{2f, -3.39999f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{0, Transform2D{{2f, -88f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{0, Transform2D{{63f, -54f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{0, Transform2D{{94f, 26f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{0, Transform2D{{66f, 79f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{0, Transform2D{{11f, 93f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{0, Transform2D{{-72f, 52f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{0, Transform2D{{-90f, -46f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{0, Transform2D{{0f, -85f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{6, Transform2D{{-224f, 52f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{9, Transform2D{{89f, -84f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{10, Transform2D{{118f, -67f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{5, Transform2D{{44f, 92f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{12, Transform2D{{11f, -104f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{3, Transform2D{{86f, -68f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{14, Transform2D{{-113f, -14f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{1, Transform2D{{-72f, -72f}, 0f, {1.f, 1.f}}});
  skeleton.bones.push_back(Bone{16, Transform2D{{-20f, 113f}, 0f, {1.f, 1.f}}});
  world.skeletons.push_back(skeleton);
  AnimationInstance instance;
  instance.skeletonId = skeletonId;
  instance.transform = Transform2D{{GetScreenWidth() / 2.f, GetScreenHeight() / 2.f}, 0.f, {1.f, 1.f}};
  instance.moveAmount = 1.f;
  const uint32_t instanceId = static_cast<uint32_t>(world.instances.size());
  world.instances.push_back(instance);
  // SpineBody
  SpineBodyRenderParams spineBody;
  spineBody.bones = {0, 9};
  spineBody.color = Color{255, 255, 210, 190};
  spineBody.outlineColor = Color{210, 255, 245, 255};
  spineBody.widths.assign(spineBody.bones.count, 24f);
  world.instances[instanceId].hasSpineBody = true;
  world.instances[instanceId].spineBodyId = addSpineBody(world, spineBody);
  // Appendage
  AppendageRenderParams appendage;
  appendage.bones = {1, 8};
  appendage.color = Color{80, 220, 210, 190};
  appendage.outlineColor = Color{210, 255, 245, 255};
  appendage.widths.assign(appendage.bones.count, 8f);
  world.instances[instanceId].appendageIds.push_back(addAppendage(world, appendage));
  // CircleBody
  CircleBodyRenderParams circleBody;
  circleBody.bone = 0;
  circleBody.radius = 50f;
  circleBody.color = Color{80, 220, 210, 190};
  circleBody.outlineColor = Color{210, 255, 245, 255};
  world.instances[instanceId].hasCircleBody = true;
  world.instances[instanceId].circleBodyId = addCircleBody(world, circleBody);
}

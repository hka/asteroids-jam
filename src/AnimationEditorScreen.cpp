#include "screens.h"

#include "raymath.h"
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace {

constexpr float kPickRadius = 12.f;
constexpr size_t kUndoLimit = 64;
constexpr const char* kRigPath = "data/animation_editor_rig.json";
constexpr const char* kExportPath = "data/animation_editor_factory.cpp";
constexpr float kLeftPanelWidth = 360.f;
constexpr float kRightPanelWidth = 400.f;
constexpr float kUiSpacing = 2.0f;
constexpr int kUiFont = 17;
constexpr int kUiSmallFont = 16;
constexpr int kUiTitleFont = 21;

Vector2 rotate(Vector2 v, float angle) {
  const float c = std::cos(angle);
  const float s = std::sin(angle);
  return {v.x * c - v.y * s, v.x * s + v.y * c};
}

float distanceSquared(Vector2 a, Vector2 b) {
  const float dx = a.x - b.x;
  const float dy = a.y - b.y;
  return dx * dx + dy * dy;
}

void drawPanel(Rectangle rect, const char* title) {
  DrawRectangleRec(rect, Color{14, 18, 26, 252});
  DrawRectangleLinesEx(rect, 1.f, Color{92, 122, 144, 255});
  DrawTextEx(GetFontDefault(), title, {rect.x + 12.f, rect.y + 10.f}, static_cast<float>(kUiTitleFont), kUiSpacing, RAYWHITE);
}

float textWidth(const char* text, int fontSize = kUiSmallFont) {
  return MeasureTextEx(GetFontDefault(), text, static_cast<float>(fontSize), kUiSpacing).x;
}

Rectangle buttonRect(float x, float y, const char* text, int fontSize = kUiSmallFont, float paddingX = 10.f, float minWidth = 0.f) {
  const float width = std::max(minWidth, textWidth(text, fontSize) + paddingX * 2.f);
  return {x, y, width, static_cast<float>(fontSize) + 12.f};
}

void drawButton(Rectangle rect, const char* text, bool active = false, int fontSize = kUiSmallFont) {
  DrawRectangleRec(rect, active ? Color{58, 86, 108, 255} : Color{34, 48, 64, 255});
  DrawRectangleLinesEx(rect, 1.f, active ? SKYBLUE : Color{118, 146, 166, 255});
  DrawTextEx(GetFontDefault(), text, {rect.x + 9.f, rect.y + 6.f}, static_cast<float>(fontSize), kUiSpacing, RAYWHITE);
}

void drawUiText(const char* text, float x, float y, Color color = LIGHTGRAY, int fontSize = kUiSmallFont) {
  DrawTextEx(GetFontDefault(), text, {x, y}, static_cast<float>(fontSize), kUiSpacing, color);
}

float drawWrappedUiText(const char* text, float x, float y, float maxWidth, Color color = LIGHTGRAY, int fontSize = kUiSmallFont) {
  std::istringstream stream(text);
  std::string word;
  std::string line;
  const float lineHeight = static_cast<float>(fontSize) + 6.f;
  while (stream >> word) {
    const std::string candidate = line.empty() ? word : line + " " + word;
    if (MeasureTextEx(GetFontDefault(), candidate.c_str(), static_cast<float>(fontSize), kUiSpacing).x > maxWidth && !line.empty()) {
      DrawTextEx(GetFontDefault(), line.c_str(), {x, y}, static_cast<float>(fontSize), kUiSpacing, color);
      y += lineHeight;
      line = word;
    } else {
      line = candidate;
    }
  }
  if (!line.empty()) {
    DrawTextEx(GetFontDefault(), line.c_str(), {x, y}, static_cast<float>(fontSize), kUiSpacing, color);
    y += lineHeight;
  }
  return y;
}

std::string formatFloat(float value) {
  char buffer[32];
  std::snprintf(buffer, sizeof(buffer), "%.3g", value);
  return buffer;
}

std::string formatInt(int value) {
  char buffer[32];
  std::snprintf(buffer, sizeof(buffer), "%d", value);
  return buffer;
}

bool parseFloat(const std::string& text, float& value) {
  char* end = nullptr;
  const float parsed = std::strtof(text.c_str(), &end);
  if (end == text.c_str()) return false;
  value = parsed;
  return true;
}

bool parseInt(const std::string& text, int& value) {
  char* end = nullptr;
  const long parsed = std::strtol(text.c_str(), &end, 10);
  if (end == text.c_str()) return false;
  value = static_cast<int>(parsed);
  return true;
}

int clampByte(int value) {
  return std::clamp(value, 0, 255);
}

const char* componentName(AnimationEditorScreen::ComponentType type) {
  switch (type) {
    case AnimationEditorScreen::ComponentType::RotateToVelocity: return "RotateToVelocity";
    case AnimationEditorScreen::ComponentType::LookAt: return "LookAt";
    case AnimationEditorScreen::ComponentType::BendChain: return "BendChain";
    case AnimationEditorScreen::ComponentType::WaveChain: return "WaveChain";
    case AnimationEditorScreen::ComponentType::Pulse: return "Pulse";
    case AnimationEditorScreen::ComponentType::SpringChain: return "SpringChain";
  }
  return "Unknown";
}

const char* componentTypeToString(AnimationEditorScreen::ComponentType type) {
  return componentName(type);
}

bool componentTypeFromName(const std::string& name, AnimationEditorScreen::ComponentType& type) {
  for (int i = 0; i <= 5; ++i) {
    const auto candidate = static_cast<AnimationEditorScreen::ComponentType>(i);
    if (name == componentTypeToString(candidate)) {
      type = candidate;
      return true;
    }
  }
  return false;
}

AnimationEditorScreen::ComponentType componentTypeByIndex(int index) {
  return static_cast<AnimationEditorScreen::ComponentType>(std::clamp(index, 0, 5));
}

AnimationEditorScreen::RenderType renderTypeByIndex(int index) {
  return static_cast<AnimationEditorScreen::RenderType>(std::clamp(index, 0, 2));
}

const char* renderName(AnimationEditorScreen::RenderType type) {
  switch (type) {
    case AnimationEditorScreen::RenderType::SpineBody: return "SpineBody";
    case AnimationEditorScreen::RenderType::Appendage: return "Appendage";
    case AnimationEditorScreen::RenderType::CircleBody: return "CircleBody";
  }
  return "Unknown";
}

const char* renderTypeToString(AnimationEditorScreen::RenderType type) {
  return renderName(type);
}

bool renderTypeFromName(const std::string& name, AnimationEditorScreen::RenderType& type) {
  for (int i = 0; i <= 2; ++i) {
    const auto candidate = static_cast<AnimationEditorScreen::RenderType>(i);
    if (name == renderTypeToString(candidate)) {
      type = candidate;
      return true;
    }
  }
  return false;
}

json colorToJson(Color color) {
  return json::array({color.r, color.g, color.b, color.a});
}

Color colorFromJson(const json& value, Color fallback) {
  if (!value.is_array() || value.size() < 4) return fallback;
  return Color{
    static_cast<unsigned char>(clampByte(value[0].get<int>())),
    static_cast<unsigned char>(clampByte(value[1].get<int>())),
    static_cast<unsigned char>(clampByte(value[2].get<int>())),
    static_cast<unsigned char>(clampByte(value[3].get<int>()))
  };
}

const char* panelModeName(AnimationEditorScreen::PanelMode mode) {
  switch (mode) {
    case AnimationEditorScreen::PanelMode::Bone: return "Bones";
    case AnimationEditorScreen::PanelMode::Component: return "Components";
    case AnimationEditorScreen::PanelMode::Render: return "Render";
    case AnimationEditorScreen::PanelMode::Export: return "Export";
  }
  return "Panel";
}

AnimationEditorScreen::ComponentType componentTypeFromInt(int value) {
  return static_cast<AnimationEditorScreen::ComponentType>(std::clamp(value, 0, 5));
}

AnimationEditorScreen::RenderType renderTypeFromInt(int value) {
  return static_cast<AnimationEditorScreen::RenderType>(std::clamp(value, 0, 2));
}

AnimationEditorScreen::ComponentType componentTypeFromJson(const json& value, AnimationEditorScreen::ComponentType fallback) {
  if (value.is_string()) {
    AnimationEditorScreen::ComponentType parsed = fallback;
    if (componentTypeFromName(value.get<std::string>(), parsed)) return parsed;
    return fallback;
  }
  if (value.is_number_integer()) return componentTypeFromInt(value.get<int>());
  return fallback;
}

AnimationEditorScreen::RenderType renderTypeFromJson(const json& value, AnimationEditorScreen::RenderType fallback) {
  if (value.is_string()) {
    AnimationEditorScreen::RenderType parsed = fallback;
    if (renderTypeFromName(value.get<std::string>(), parsed)) return parsed;
    return fallback;
  }
  if (value.is_number_integer()) return renderTypeFromInt(value.get<int>());
  return fallback;
}

} // namespace

AnimationEditorScreen::AnimationEditorScreen()
{
  m_previewTransform = Transform2D{{GetScreenWidth() * 0.55f, GetScreenHeight() * 0.52f}, 0.f, {1.f, 1.f}};
  importSpineSample(false);
}

AnimationEditorScreen::~AnimationEditorScreen()
{
}

bool AnimationEditorScreen::isEditingText() const
{
  return m_activeParamInput >= 0;
}

Skeleton AnimationEditorScreen::buildSkeleton() const
{
  Skeleton skeleton;
  skeleton.bones.reserve(m_bones.size());
  for (const EditableBone& bone : m_bones) {
    skeleton.bones.push_back(Bone{bone.parent, bone.bindLocal});
  }
  return skeleton;
}

void AnimationEditorScreen::syncEditableRanges()
{
  if (m_bones.empty()) return;

  const int maxBone = static_cast<int>(m_bones.size() - 1);
  auto clampRange = [&](BoneRange& range) {
    range.start = static_cast<uint16_t>(std::clamp<int>(range.start, 0, maxBone));
    const int maxCount = static_cast<int>(m_bones.size()) - range.start;
    range.count = static_cast<uint16_t>(std::clamp<int>(range.count <= 0 ? 1 : range.count, 1, maxCount));
  };

  for (EditableComponent& component : m_components) {
    component.bone = static_cast<uint16_t>(std::clamp<int>(component.bone, 0, maxBone));
    clampRange(component.bones);
  }

  for (EditableRenderShape& shape : m_renderShapes) {
    shape.bone = static_cast<uint16_t>(std::clamp<int>(shape.bone, 0, maxBone));
    clampRange(shape.bones);
  }
}

AnimationEditorScreen::EditorSnapshot AnimationEditorScreen::captureSnapshot() const
{
  return EditorSnapshot{m_bones, m_components, m_renderShapes, m_selectedBone, m_selectedComponent, m_selectedRenderShape, m_panelMode, m_snapToGrid};
}

void AnimationEditorScreen::restoreSnapshot(const EditorSnapshot& snapshot)
{
  m_bones = snapshot.bones;
  m_components = snapshot.components;
  m_renderShapes = snapshot.renderShapes;
  m_selectedBone = snapshot.selectedBone;
  m_selectedComponent = snapshot.selectedComponent;
  m_selectedRenderShape = snapshot.selectedRenderShape;
  m_panelMode = snapshot.panelMode;
  m_snapToGrid = snapshot.snapToGrid;
  m_activeParamInput = -1;
  m_componentDropdownOpen = false;
  m_renderDropdownOpen = false;
  m_draggingBone = false;
  syncEditableRanges();
  m_dirtyPreview = true;
}

void AnimationEditorScreen::pushUndoSnapshot()
{
  if (m_suppressUndo) return;
  m_undoStack.push_back(captureSnapshot());
  if (m_undoStack.size() > kUndoLimit) m_undoStack.erase(m_undoStack.begin());
  m_redoStack.clear();
}

void AnimationEditorScreen::undo()
{
  if (m_undoStack.empty()) return;
  m_redoStack.push_back(captureSnapshot());
  EditorSnapshot snapshot = m_undoStack.back();
  m_undoStack.pop_back();
  restoreSnapshot(snapshot);
  m_statusText = "Undo";
}

void AnimationEditorScreen::redo()
{
  if (m_redoStack.empty()) return;
  m_undoStack.push_back(captureSnapshot());
  if (m_undoStack.size() > kUndoLimit) m_undoStack.erase(m_undoStack.begin());
  EditorSnapshot snapshot = m_redoStack.back();
  m_redoStack.pop_back();
  restoreSnapshot(snapshot);
  m_statusText = "Redo";
}

void AnimationEditorScreen::addComponent(ComponentType type)
{
  pushUndoSnapshot();
  EditableComponent component;
  component.type = type;
  component.bones = {m_bones.size() > 1 ? static_cast<uint16_t>(1) : static_cast<uint16_t>(0), static_cast<uint16_t>(std::max<size_t>(1, m_bones.size() > 1 ? m_bones.size() - 1 : m_bones.size()))};
  component.bone = static_cast<uint16_t>(std::clamp(m_selectedBone, 0, static_cast<int>(std::max<size_t>(1, m_bones.size()) - 1)));
  m_components.push_back(component);
  m_selectedComponent = static_cast<int>(m_components.size() - 1);
  m_panelMode = PanelMode::Component;
  m_activeParamInput = -1;
  m_dirtyPreview = true;
  m_statusText = std::string("Added ") + componentName(type);
}

void AnimationEditorScreen::addRenderShape(RenderType type)
{
  pushUndoSnapshot();
  EditableRenderShape shape;
  shape.type = type;
  shape.bones = {0, static_cast<uint16_t>(std::max<size_t>(1, m_bones.size()))};
  shape.bone = static_cast<uint16_t>(std::clamp(m_selectedBone, 0, static_cast<int>(std::max<size_t>(1, m_bones.size()) - 1)));
  if (type == RenderType::Appendage) shape.width = 8.f;
  if (type == RenderType::CircleBody) shape.radius = 26.f;
  m_renderShapes.push_back(shape);
  m_selectedRenderShape = static_cast<int>(m_renderShapes.size() - 1);
  m_panelMode = PanelMode::Render;
  m_activeParamInput = -1;
  m_dirtyPreview = true;
  m_statusText = std::string("Added ") + renderName(type);
}

void AnimationEditorScreen::deleteSelectedComponent()
{
  if (m_selectedComponent < 0 || m_selectedComponent >= static_cast<int>(m_components.size())) return;
  pushUndoSnapshot();
  m_components.erase(m_components.begin() + m_selectedComponent);
  m_selectedComponent = m_components.empty() ? -1 : std::min<int>(m_selectedComponent, static_cast<int>(m_components.size() - 1));
  m_activeParamInput = -1;
  m_dirtyPreview = true;
}

void AnimationEditorScreen::duplicateSelectedComponent()
{
  if (m_selectedComponent < 0 || m_selectedComponent >= static_cast<int>(m_components.size())) return;

  pushUndoSnapshot();
  const int insertIndex = m_selectedComponent + 1;
  m_components.insert(m_components.begin() + insertIndex, m_components[m_selectedComponent]);
  m_selectedComponent = insertIndex;
  m_activeParamInput = -1;
  m_dirtyPreview = true;
  m_statusText = "Duplicated component";
}

void AnimationEditorScreen::moveSelectedComponent(int direction)
{
  if (m_selectedComponent < 0 || m_selectedComponent >= static_cast<int>(m_components.size())) return;
  const int targetIndex = m_selectedComponent + direction;
  if (targetIndex < 0 || targetIndex >= static_cast<int>(m_components.size())) return;

  pushUndoSnapshot();
  std::swap(m_components[m_selectedComponent], m_components[targetIndex]);
  m_selectedComponent = targetIndex;
  m_activeParamInput = -1;
  m_dirtyPreview = true;
  m_statusText = "Moved component";
}

void AnimationEditorScreen::deleteSelectedRenderShape()
{
  if (m_selectedRenderShape < 0 || m_selectedRenderShape >= static_cast<int>(m_renderShapes.size())) return;
  pushUndoSnapshot();
  m_renderShapes.erase(m_renderShapes.begin() + m_selectedRenderShape);
  m_selectedRenderShape = m_renderShapes.empty() ? -1 : std::min<int>(m_selectedRenderShape, static_cast<int>(m_renderShapes.size() - 1));
  m_activeParamInput = -1;
  m_dirtyPreview = true;
}

void AnimationEditorScreen::rebuildPreview()
{
  m_previewWorld = AnimationWorld{};
  if (m_bones.empty()) {
    m_dirtyPreview = false;
    return;
  }

  syncEditableRanges();
  m_previewWorld.skeletons.push_back(buildSkeleton());

  AnimationInstance instance;
  instance.skeletonId = 0;
  instance.transform = m_previewTransform;
  instance.moveAmount = 1.f;
  instance.moveSpeed = 180.f;
  instance.velocity = {180.f, 0.f};
  instance.turnAmount = 0.6f;
  instance.aimTarget = {m_previewTransform.position.x + 150.f, m_previewTransform.position.y - 60.f};

  m_previewWorld.instances.push_back(instance);

  for (const EditableComponent& component : m_components) {
    if (!component.enabled) continue;
    switch (component.type) {
      case ComponentType::RotateToVelocity:
        addRotateToVelocity(m_previewWorld, 0, RotateToVelocityParams{component.turnSpeed}, component.weight);
        break;
      case ComponentType::LookAt: {
        LookAtParams params;
        params.bone = component.bone;
        params.target = m_previewWorld.instances[0].aimTarget;
        params.weight = component.lookWeight;
        addLookAt(m_previewWorld, 0, params, component.weight);
        break;
      }
      case ComponentType::BendChain:
        addBendChain(m_previewWorld, 0, BendChainParams{component.bones, component.maxAngle, component.distribution}, component.weight);
        break;
      case ComponentType::WaveChain: {
        WaveChainParams params;
        params.bones = component.bones;
        params.idleAmplitude = component.idleAmplitude;
        params.idleFrequency = component.idleFrequency;
        params.moveAmplitude = component.moveAmplitude;
        params.moveFrequency = component.moveFrequency;
        params.phaseOffset = component.phaseOffset;
        addWaveChain(m_previewWorld, 0, params, component.weight);
        break;
      }
      case ComponentType::Pulse: {
        PulseParams params;
        params.bones = component.bones;
        params.amplitude = component.amplitude;
        params.frequency = component.frequency;
        params.phaseOffset = component.phaseOffset;
        addPulse(m_previewWorld, 0, params, component.weight);
        break;
      }
      case ComponentType::SpringChain:
        addSpringChain(m_previewWorld, 0, SpringChainParams{component.bones, component.stiffness, component.damping}, component.weight);
        break;
    }
  }

  for (const EditableRenderShape& shape : m_renderShapes) {
    if (!shape.enabled) continue;
    switch (shape.type) {
      case RenderType::SpineBody: {
        SpineBodyRenderParams params;
        params.bones = shape.bones;
        params.color = shape.color;
        params.outlineColor = shape.outlineColor;
        params.widths.assign(shape.bones.count, shape.width);
        m_previewWorld.instances[0].hasSpineBody = true;
        m_previewWorld.instances[0].spineBodyId = addSpineBody(m_previewWorld, params);
        break;
      }
      case RenderType::Appendage: {
        AppendageRenderParams params;
        params.bones = shape.bones;
        params.color = shape.color;
        params.outlineColor = shape.outlineColor;
        params.widths.assign(shape.bones.count, shape.width);
        m_previewWorld.instances[0].appendageIds.push_back(addAppendage(m_previewWorld, params));
        break;
      }
      case RenderType::CircleBody: {
        CircleBodyRenderParams params;
        params.bone = shape.bone;
        params.radius = shape.radius;
        params.color = shape.color;
        params.outlineColor = shape.outlineColor;
        m_previewWorld.instances[0].hasCircleBody = true;
        m_previewWorld.instances[0].circleBodyId = addCircleBody(m_previewWorld, params);
        break;
      }
    }
  }

  updateAnimationWorld(m_previewWorld, 0.f);
  m_dirtyPreview = false;
}

void AnimationEditorScreen::ensurePreviewPose()
{
  if (m_dirtyPreview) rebuildPreview();
}

int AnimationEditorScreen::pickBone(Vector2 point) const
{
  if (m_previewWorld.instances.empty()) return -1;
  const Pose& pose = m_previewWorld.instances[0].pose;
  int best = -1;
  float bestDistance = kPickRadius * kPickRadius;
  for (size_t i = 0; i < pose.world.size(); ++i) {
    const float d = distanceSquared(point, pose.world[i].position);
    if (d <= bestDistance) {
      bestDistance = d;
      best = static_cast<int>(i);
    }
  }
  return best;
}

Vector2 AnimationEditorScreen::parentLocalPosition(int parent, Vector2 worldPosition) const
{
  Vector2 position = worldPosition;
  if (m_snapToGrid) {
    position.x = std::round(position.x / 10.f) * 10.f;
    position.y = std::round(position.y / 10.f) * 10.f;
  }

  if (parent < 0 || m_previewWorld.instances.empty()) {
    const Vector2 diff = Vector2Subtract(position, m_previewTransform.position);
    return rotate(diff, -m_previewTransform.rotation);
  }

  const Pose& pose = m_previewWorld.instances[0].pose;
  if (static_cast<size_t>(parent) >= pose.world.size()) return {0.f, 0.f};
  const Transform2D& parentWorld = pose.world[parent];
  const Vector2 diff = Vector2Subtract(position, parentWorld.position);
  return rotate(diff, -parentWorld.rotation);
}

bool AnimationEditorScreen::isDescendant(int possibleDescendant, int ancestor) const
{
  if (possibleDescendant < 0 || ancestor < 0) return false;
  if (possibleDescendant >= static_cast<int>(m_bones.size()) || ancestor >= static_cast<int>(m_bones.size())) return false;

  std::vector<bool> visited(m_bones.size(), false);
  int current = possibleDescendant;
  while (current >= 0 && current < static_cast<int>(m_bones.size()) && !visited[current]) {
    if (current == ancestor) return true;
    visited[current] = true;
    current = m_bones[current].parent;
  }
  return false;
}

bool AnimationEditorScreen::canReparentBone(int boneIndex, int newParent) const
{
  if (boneIndex < 0 || boneIndex >= static_cast<int>(m_bones.size())) return false;
  if (newParent == -1) return true;
  if (newParent < -1 || newParent >= static_cast<int>(m_bones.size())) return false;
  if (newParent == boneIndex) return false;
  return !isDescendant(newParent, boneIndex);
}

void AnimationEditorScreen::createBone(Vector2 worldPosition)
{
  ensurePreviewPose();
  pushUndoSnapshot();
  EditableBone bone;
  bone.parent = m_bones.empty() ? -1 : std::max(0, m_selectedBone);
  bone.bindLocal.position = parentLocalPosition(bone.parent, worldPosition);
  bone.bindLocal.rotation = 0.f;
  bone.bindLocal.scale = {1.f, 1.f};
  bone.name = TextFormat("bone%zu", m_bones.size());
  m_bones.push_back(bone);
  m_selectedBone = static_cast<int>(m_bones.size() - 1);
  m_dirtyPreview = true;
}

void AnimationEditorScreen::duplicateSelectedBone()
{
  if (m_selectedBone < 0 || m_selectedBone >= static_cast<int>(m_bones.size())) return;
  pushUndoSnapshot();
  EditableBone bone = m_bones[m_selectedBone];
  bone.bindLocal.position.x -= 24.f;
  bone.name += "_copy";
  m_bones.push_back(bone);
  m_selectedBone = static_cast<int>(m_bones.size() - 1);
  m_dirtyPreview = true;
}

void AnimationEditorScreen::deleteSelectedBone()
{
  if (m_selectedBone < 0 || static_cast<size_t>(m_selectedBone) >= m_bones.size()) return;
  pushUndoSnapshot();
  std::vector<bool> remove(m_bones.size(), false);
  remove[m_selectedBone] = true;
  bool changed = true;
  while (changed) {
    changed = false;
    for (size_t i = 0; i < m_bones.size(); ++i) {
      const int parent = m_bones[i].parent;
      if (!remove[i] && parent >= 0 && remove[parent]) {
        remove[i] = true;
        changed = true;
      }
    }
  }

  std::vector<int> remap(m_bones.size(), -1);
  std::vector<EditableBone> kept;
  for (size_t i = 0; i < m_bones.size(); ++i) {
    if (remove[i]) continue;
    remap[i] = static_cast<int>(kept.size());
    kept.push_back(m_bones[i]);
  }
  for (EditableBone& bone : kept) {
    if (bone.parent >= 0) bone.parent = remap[bone.parent];
  }
  for (EditableComponent& component : m_components) {
    if (component.bone < remap.size() && remap[component.bone] >= 0) component.bone = static_cast<uint16_t>(remap[component.bone]);
  }
  for (EditableRenderShape& shape : m_renderShapes) {
    if (shape.bone < remap.size() && remap[shape.bone] >= 0) shape.bone = static_cast<uint16_t>(remap[shape.bone]);
  }

  m_bones = kept;
  m_selectedBone = m_bones.empty() ? -1 : std::min<int>(m_selectedBone, static_cast<int>(m_bones.size() - 1));
  syncEditableRanges();
  m_dirtyPreview = true;
}

void AnimationEditorScreen::importSpineSample(bool undoable)
{
  if (undoable) pushUndoSnapshot();
  const bool wasSuppressingUndo = m_suppressUndo;
  m_suppressUndo = true;
  m_bones.clear();
  m_components.clear();
  m_renderShapes.clear();

  const Vector2 positions[] = {{0.f, 0.f}, {30.f, 0.f}, {-38.f, 0.f}, {-40.f, 0.f}, {-34.f, 0.f}, {-24.f, 0.f}};
  const int parents[] = {-1, 0, 1, 2, 3, 4};
  for (size_t i = 0; i < 6; ++i) {
    EditableBone bone;
    bone.parent = parents[i];
    bone.bindLocal = Transform2D{positions[i], 0.f, {1.f, 1.f}};
    bone.name = i == 0 ? "root" : TextFormat("spine%zu", i);
    m_bones.push_back(bone);
  }

  addComponent(ComponentType::BendChain);
  m_components.back().bones = {1, 5};
  m_components.back().maxAngle = 0.25f;
  addComponent(ComponentType::WaveChain);
  m_components.back().bones = {2, 4};
  m_components.back().moveAmplitude = 0.12f;
  m_components.back().moveFrequency = 5.f;
  addComponent(ComponentType::Pulse);
  m_components.back().bones = {1, 5};
  m_components.back().amplitude = 0.05f;
  addComponent(ComponentType::SpringChain);
  m_components.back().bones = {1, 5};
  m_components.back().stiffness = 55.f;
  m_components.back().damping = 10.f;
  addRenderShape(RenderType::SpineBody);
  m_renderShapes.back().bones = {0, 6};
  m_renderShapes.back().width = 14.f;
  m_suppressUndo = wasSuppressingUndo;

  m_selectedBone = 1;
  m_selectedComponent = 1;
  m_selectedRenderShape = 0;
  m_panelMode = PanelMode::Component;
  m_activeParamInput = -1;
  m_dirtyPreview = true;
  m_statusText = "Imported spine sample";
}

void AnimationEditorScreen::rebuildParamInputs()
{
  m_paramInputs.clear();

  auto add = [&](ParamTarget target, ParamId id, const char* label) {
    m_paramInputs.push_back({target, id, label, {}, ""});
  };

  if (m_panelMode == PanelMode::Component && m_selectedComponent >= 0 && m_selectedComponent < static_cast<int>(m_components.size())) {
    const EditableComponent& component = m_components[m_selectedComponent];
    if (component.type == ComponentType::LookAt) add(ParamTarget::Component, ParamId::BoneIndex, "Bone");
    if (component.type == ComponentType::RotateToVelocity) add(ParamTarget::Component, ParamId::TurnSpeed, "Turn speed");
    if (component.type == ComponentType::BendChain || component.type == ComponentType::WaveChain || component.type == ComponentType::Pulse || component.type == ComponentType::SpringChain) {
      add(ParamTarget::Component, ParamId::RangeStart, "Start bone");
      add(ParamTarget::Component, ParamId::RangeCount, "Bone count");
    }
    if (component.type == ComponentType::BendChain) {
      add(ParamTarget::Component, ParamId::BendMaxAngle, "Max angle");
      add(ParamTarget::Component, ParamId::BendDistribution, "Distribution");
    }
    if (component.type == ComponentType::WaveChain) {
      add(ParamTarget::Component, ParamId::WaveIdleAmplitude, "Idle amp");
      add(ParamTarget::Component, ParamId::WaveIdleFrequency, "Idle freq");
      add(ParamTarget::Component, ParamId::WaveMoveAmplitude, "Move amp");
      add(ParamTarget::Component, ParamId::WaveMoveFrequency, "Move freq");
      add(ParamTarget::Component, ParamId::WavePhaseOffset, "Phase offset");
    }
    if (component.type == ComponentType::Pulse) {
      add(ParamTarget::Component, ParamId::PulseAmplitude, "Amplitude");
      add(ParamTarget::Component, ParamId::PulseFrequency, "Frequency");
      add(ParamTarget::Component, ParamId::PulsePhaseOffset, "Phase offset");
    }
    if (component.type == ComponentType::SpringChain) {
      add(ParamTarget::Component, ParamId::SpringStiffness, "Stiffness");
      add(ParamTarget::Component, ParamId::SpringDamping, "Damping");
    }
    if (component.type == ComponentType::LookAt) add(ParamTarget::Component, ParamId::LookWeight, "Look weight");
    add(ParamTarget::Component, ParamId::Weight, "Weight");
  } else if (m_panelMode == PanelMode::Render && m_selectedRenderShape >= 0 && m_selectedRenderShape < static_cast<int>(m_renderShapes.size())) {
    const EditableRenderShape& shape = m_renderShapes[m_selectedRenderShape];
    if (shape.type == RenderType::CircleBody) {
      add(ParamTarget::Render, ParamId::BoneIndex, "Bone");
      add(ParamTarget::Render, ParamId::RenderRadius, "Radius");
    } else {
      add(ParamTarget::Render, ParamId::RangeStart, "Start bone");
      add(ParamTarget::Render, ParamId::RangeCount, "Bone count");
      add(ParamTarget::Render, ParamId::RenderWidth, "Width");
    }
    add(ParamTarget::Render, ParamId::ColorR, "Color R");
    add(ParamTarget::Render, ParamId::ColorG, "Color G");
    add(ParamTarget::Render, ParamId::ColorB, "Color B");
    add(ParamTarget::Render, ParamId::ColorA, "Color A");
  } else if (m_panelMode == PanelMode::Bone && m_selectedBone >= 0 && m_selectedBone < static_cast<int>(m_bones.size())) {
    add(ParamTarget::Bone, ParamId::BoneName, "Bone name");
    add(ParamTarget::Bone, ParamId::BoneParent, "Parent");
    add(ParamTarget::Bone, ParamId::BoneX, "Local X");
    add(ParamTarget::Bone, ParamId::BoneY, "Local Y");
    add(ParamTarget::Bone, ParamId::BoneRotation, "Rotation");
  }

  syncParamInputs();
}

void AnimationEditorScreen::layoutParamInputs(Rectangle panel)
{
  const bool hadActive = m_activeParamInput >= 0 && m_activeParamInput < static_cast<int>(m_paramInputs.size());
  ParamTarget activeTarget = ParamTarget::Component;
  ParamId activeId = ParamId::Weight;
  std::string activeText;
  if (hadActive) {
    activeTarget = m_paramInputs[m_activeParamInput].target;
    activeId = m_paramInputs[m_activeParamInput].id;
    activeText = m_paramInputs[m_activeParamInput].text;
  }

  m_activeParamInput = -1;
  rebuildParamInputs();
  if (hadActive) {
    for (size_t i = 0; i < m_paramInputs.size(); ++i) {
      if (m_paramInputs[i].target == activeTarget && m_paramInputs[i].id == activeId) {
        m_activeParamInput = static_cast<int>(i);
        m_paramInputs[i].text = activeText;
        break;
      }
    }
  }

  float y = panel.y + 208.f;
  if (m_panelMode == PanelMode::Component) y = panel.y + 352.f;
  if (m_panelMode == PanelMode::Render) y = panel.y + 324.f;
  if (m_panelMode == PanelMode::Export) y = panel.y + 128.f;
  for (ParamInput& input : m_paramInputs) {
    input.bounds = {panel.x + 156.f, y, 190.f, 26.f};
    y += 34.f;
  }
}

void AnimationEditorScreen::syncParamInputs()
{
  for (size_t i = 0; i < m_paramInputs.size(); ++i) {
    if (static_cast<int>(i) == m_activeParamInput) continue;
    ParamInput& input = m_paramInputs[i];

    if (input.target == ParamTarget::Bone && m_selectedBone >= 0 && m_selectedBone < static_cast<int>(m_bones.size())) {
      const EditableBone& bone = m_bones[m_selectedBone];
      if (input.id == ParamId::BoneName) input.text = bone.name;
      if (input.id == ParamId::BoneParent) input.text = formatInt(bone.parent);
      if (input.id == ParamId::BoneX) input.text = formatFloat(bone.bindLocal.position.x);
      if (input.id == ParamId::BoneY) input.text = formatFloat(bone.bindLocal.position.y);
      if (input.id == ParamId::BoneRotation) input.text = formatFloat(bone.bindLocal.rotation);
    } else if (input.target == ParamTarget::Component && m_selectedComponent >= 0 && m_selectedComponent < static_cast<int>(m_components.size())) {
      const EditableComponent& c = m_components[m_selectedComponent];
      if (input.id == ParamId::RangeStart) input.text = formatInt(c.bones.start);
      if (input.id == ParamId::RangeCount) input.text = formatInt(c.bones.count);
      if (input.id == ParamId::BoneIndex) input.text = formatInt(c.bone);
      if (input.id == ParamId::Weight) input.text = formatFloat(c.weight);
      if (input.id == ParamId::TurnSpeed) input.text = formatFloat(c.turnSpeed);
      if (input.id == ParamId::LookWeight) input.text = formatFloat(c.lookWeight);
      if (input.id == ParamId::BendMaxAngle) input.text = formatFloat(c.maxAngle);
      if (input.id == ParamId::BendDistribution) input.text = formatFloat(c.distribution);
      if (input.id == ParamId::WaveIdleAmplitude) input.text = formatFloat(c.idleAmplitude);
      if (input.id == ParamId::WaveIdleFrequency) input.text = formatFloat(c.idleFrequency);
      if (input.id == ParamId::WaveMoveAmplitude) input.text = formatFloat(c.moveAmplitude);
      if (input.id == ParamId::WaveMoveFrequency) input.text = formatFloat(c.moveFrequency);
      if (input.id == ParamId::WavePhaseOffset) input.text = formatFloat(c.phaseOffset);
      if (input.id == ParamId::PulseAmplitude) input.text = formatFloat(c.amplitude);
      if (input.id == ParamId::PulseFrequency) input.text = formatFloat(c.frequency);
      if (input.id == ParamId::PulsePhaseOffset) input.text = formatFloat(c.phaseOffset);
      if (input.id == ParamId::SpringStiffness) input.text = formatFloat(c.stiffness);
      if (input.id == ParamId::SpringDamping) input.text = formatFloat(c.damping);
    } else if (input.target == ParamTarget::Render && m_selectedRenderShape >= 0 && m_selectedRenderShape < static_cast<int>(m_renderShapes.size())) {
      const EditableRenderShape& r = m_renderShapes[m_selectedRenderShape];
      if (input.id == ParamId::RangeStart) input.text = formatInt(r.bones.start);
      if (input.id == ParamId::RangeCount) input.text = formatInt(r.bones.count);
      if (input.id == ParamId::BoneIndex) input.text = formatInt(r.bone);
      if (input.id == ParamId::RenderWidth) input.text = formatFloat(r.width);
      if (input.id == ParamId::RenderRadius) input.text = formatFloat(r.radius);
      if (input.id == ParamId::ColorR) input.text = formatInt(r.color.r);
      if (input.id == ParamId::ColorG) input.text = formatInt(r.color.g);
      if (input.id == ParamId::ColorB) input.text = formatInt(r.color.b);
      if (input.id == ParamId::ColorA) input.text = formatInt(r.color.a);
    }
  }
}

void AnimationEditorScreen::applyParamInput(ParamInput& input)
{
  int intValue = 0;
  float floatValue = 0.f;
  const int maxBone = static_cast<int>(std::max<size_t>(1, m_bones.size()) - 1);
  bool pushed = false;
  auto pushBeforeChange = [&]() {
    if (!pushed) {
      pushUndoSnapshot();
      pushed = true;
    }
  };
  auto setFloat = [&](float& target, float value) {
    if (target != value) {
      pushBeforeChange();
      target = value;
    }
  };
  auto setUInt16 = [&](uint16_t& target, int value) {
    const uint16_t newValue = static_cast<uint16_t>(value);
    if (target != newValue) {
      pushBeforeChange();
      target = newValue;
    }
  };
  auto setByte = [&](unsigned char& target, int value) {
    const unsigned char newValue = static_cast<unsigned char>(value);
    if (target != newValue) {
      pushBeforeChange();
      target = newValue;
    }
  };

  if (input.target == ParamTarget::Bone && m_selectedBone >= 0 && m_selectedBone < static_cast<int>(m_bones.size())) {
    EditableBone& bone = m_bones[m_selectedBone];
    if (input.id == ParamId::BoneName && !input.text.empty() && bone.name != input.text) {
      pushBeforeChange();
      bone.name = input.text;
    }
    if (input.id == ParamId::BoneParent && parseInt(input.text, intValue)) {
      if (canReparentBone(m_selectedBone, intValue)) {
        if (bone.parent != intValue) {
          pushBeforeChange();
          bone.parent = intValue;
        }
      } else {
        if (intValue < -1 || intValue >= static_cast<int>(m_bones.size())) {
          m_statusText = "Invalid parent: index must be -1 or an existing bone";
        } else if (intValue == m_selectedBone) {
          m_statusText = "Invalid parent: a bone cannot parent itself";
        } else {
          m_statusText = "Invalid parent: would create a parent cycle";
        }
      }
    }
    if (input.id == ParamId::BoneX && parseFloat(input.text, floatValue)) setFloat(bone.bindLocal.position.x, floatValue);
    if (input.id == ParamId::BoneY && parseFloat(input.text, floatValue)) setFloat(bone.bindLocal.position.y, floatValue);
    if (input.id == ParamId::BoneRotation && parseFloat(input.text, floatValue)) setFloat(bone.bindLocal.rotation, floatValue);
  } else if (input.target == ParamTarget::Component && m_selectedComponent >= 0 && m_selectedComponent < static_cast<int>(m_components.size())) {
    EditableComponent& c = m_components[m_selectedComponent];
    if (input.id == ParamId::RangeStart && parseInt(input.text, intValue)) setUInt16(c.bones.start, std::clamp(intValue, 0, maxBone));
    if (input.id == ParamId::RangeCount && parseInt(input.text, intValue)) setUInt16(c.bones.count, std::max(1, intValue));
    if (input.id == ParamId::BoneIndex && parseInt(input.text, intValue)) setUInt16(c.bone, std::clamp(intValue, 0, maxBone));
    if (input.id == ParamId::Weight && parseFloat(input.text, floatValue)) setFloat(c.weight, floatValue);
    if (input.id == ParamId::TurnSpeed && parseFloat(input.text, floatValue)) setFloat(c.turnSpeed, std::max(0.f, floatValue));
    if (input.id == ParamId::LookWeight && parseFloat(input.text, floatValue)) setFloat(c.lookWeight, floatValue);
    if (input.id == ParamId::BendMaxAngle && parseFloat(input.text, floatValue)) setFloat(c.maxAngle, floatValue);
    if (input.id == ParamId::BendDistribution && parseFloat(input.text, floatValue)) setFloat(c.distribution, std::max(0.01f, floatValue));
    if (input.id == ParamId::WaveIdleAmplitude && parseFloat(input.text, floatValue)) setFloat(c.idleAmplitude, std::max(0.f, floatValue));
    if (input.id == ParamId::WaveIdleFrequency && parseFloat(input.text, floatValue)) setFloat(c.idleFrequency, std::max(0.f, floatValue));
    if (input.id == ParamId::WaveMoveAmplitude && parseFloat(input.text, floatValue)) setFloat(c.moveAmplitude, std::max(0.f, floatValue));
    if (input.id == ParamId::WaveMoveFrequency && parseFloat(input.text, floatValue)) setFloat(c.moveFrequency, std::max(0.f, floatValue));
    if (input.id == ParamId::WavePhaseOffset && parseFloat(input.text, floatValue)) setFloat(c.phaseOffset, floatValue);
    if (input.id == ParamId::PulseAmplitude && parseFloat(input.text, floatValue)) setFloat(c.amplitude, std::max(0.f, floatValue));
    if (input.id == ParamId::PulseFrequency && parseFloat(input.text, floatValue)) setFloat(c.frequency, std::max(0.f, floatValue));
    if (input.id == ParamId::PulsePhaseOffset && parseFloat(input.text, floatValue)) setFloat(c.phaseOffset, floatValue);
    if (input.id == ParamId::SpringStiffness && parseFloat(input.text, floatValue)) setFloat(c.stiffness, std::max(0.f, floatValue));
    if (input.id == ParamId::SpringDamping && parseFloat(input.text, floatValue)) setFloat(c.damping, std::max(0.f, floatValue));
  } else if (input.target == ParamTarget::Render && m_selectedRenderShape >= 0 && m_selectedRenderShape < static_cast<int>(m_renderShapes.size())) {
    EditableRenderShape& r = m_renderShapes[m_selectedRenderShape];
    if (input.id == ParamId::RangeStart && parseInt(input.text, intValue)) setUInt16(r.bones.start, std::clamp(intValue, 0, maxBone));
    if (input.id == ParamId::RangeCount && parseInt(input.text, intValue)) setUInt16(r.bones.count, std::max(1, intValue));
    if (input.id == ParamId::BoneIndex && parseInt(input.text, intValue)) setUInt16(r.bone, std::clamp(intValue, 0, maxBone));
    if (input.id == ParamId::RenderWidth && parseFloat(input.text, floatValue)) setFloat(r.width, std::max(0.f, floatValue));
    if (input.id == ParamId::RenderRadius && parseFloat(input.text, floatValue)) setFloat(r.radius, std::max(0.f, floatValue));
    if (input.id == ParamId::ColorR && parseInt(input.text, intValue)) setByte(r.color.r, clampByte(intValue));
    if (input.id == ParamId::ColorG && parseInt(input.text, intValue)) setByte(r.color.g, clampByte(intValue));
    if (input.id == ParamId::ColorB && parseInt(input.text, intValue)) setByte(r.color.b, clampByte(intValue));
    if (input.id == ParamId::ColorA && parseInt(input.text, intValue)) setByte(r.color.a, clampByte(intValue));
  }

  if (pushed) {
    syncEditableRanges();
    m_dirtyPreview = true;
  }
}

void AnimationEditorScreen::updateParamInputs(Vector2 mouse)
{
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    const int previousActive = m_activeParamInput;
    m_activeParamInput = -1;
    for (size_t i = 0; i < m_paramInputs.size(); ++i) {
      if (CheckCollisionPointRec(mouse, m_paramInputs[i].bounds)) {
        m_activeParamInput = static_cast<int>(i);
        break;
      }
    }
    if (previousActive >= 0 && previousActive < static_cast<int>(m_paramInputs.size()) && previousActive != m_activeParamInput) {
      applyParamInput(m_paramInputs[previousActive]);
    }
  }

  if (!isEditingText()) {
    syncParamInputs();
    return;
  }

  ParamInput& input = m_paramInputs[m_activeParamInput];
  int key = GetCharPressed();
  while (key > 0) {
    const bool textField = input.id == ParamId::BoneName;
    const bool numeric = (key >= '0' && key <= '9') || key == '-' || key == '+' || key == '.';
    if ((textField || numeric) && key >= 32 && key <= 125 && input.text.size() < 32) {
      input.text.push_back(static_cast<char>(key));
    }
    key = GetCharPressed();
  }
  if (IsKeyPressed(KEY_BACKSPACE) && !input.text.empty()) input.text.pop_back();
  if (IsKeyPressed(KEY_ENTER)) {
    applyParamInput(input);
    m_activeParamInput = -1;
  }
}

void AnimationEditorScreen::drawParamInputs() const
{
  for (size_t i = 0; i < m_paramInputs.size(); ++i) {
    const ParamInput& input = m_paramInputs[i];
    const bool active = static_cast<int>(i) == m_activeParamInput;
    drawUiText(input.label, input.bounds.x - 140.f, input.bounds.y + 5.f, LIGHTGRAY);
    DrawRectangleRec(input.bounds, active ? Color{42, 66, 84, 255} : Color{20, 28, 38, 255});
    DrawRectangleLinesEx(input.bounds, 1.f, active ? SKYBLUE : Color{118, 146, 166, 255});
    drawUiText(input.text.c_str(), input.bounds.x + 8.f, input.bounds.y + 5.f, RAYWHITE);
  }
}

bool AnimationEditorScreen::handleComponentUi(Vector2 mouse, Rectangle panel)
{
  if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return false;

  const Rectangle tabs[] = {
    Rectangle{panel.x + 12.f, panel.y + 42.f, 70.f, 24.f},
    Rectangle{panel.x + 88.f, panel.y + 42.f, 92.f, 24.f},
    Rectangle{panel.x + 186.f, panel.y + 42.f, 62.f, 24.f},
    Rectangle{panel.x + 254.f, panel.y + 42.f, 64.f, 24.f},
  };
  const PanelMode modes[] = {PanelMode::Bone, PanelMode::Component, PanelMode::Render, PanelMode::Export};
  for (size_t i = 0; i < sizeof(tabs) / sizeof(tabs[0]); ++i) {
    if (CheckCollisionPointRec(mouse, tabs[i])) {
      m_panelMode = modes[i];
      m_activeParamInput = -1;
      m_componentDropdownOpen = false;
      m_renderDropdownOpen = false;
      return true;
    }
  }

  if (m_panelMode != PanelMode::Component) return false;

  const Rectangle addButton = buttonRect(panel.x + 16.f, panel.y + 110.f, "Add Component", kUiSmallFont, 10.f, 136.f);
  const Rectangle prevButton = buttonRect(addButton.x + addButton.width + 10.f, panel.y + 110.f, "<", kUiSmallFont, 10.f, 34.f);
  const Rectangle nextButton = buttonRect(prevButton.x + prevButton.width + 6.f, panel.y + 110.f, ">", kUiSmallFont, 10.f, 34.f);
  const Rectangle duplicateButton = buttonRect(nextButton.x + nextButton.width + 8.f, panel.y + 110.f, "Duplicate", kUiSmallFont, 10.f, 92.f);
  const Rectangle upButton = buttonRect(panel.x + 16.f, panel.y + 148.f, "Up", kUiSmallFont, 10.f, 50.f);
  const Rectangle downButton = buttonRect(upButton.x + upButton.width + 8.f, panel.y + 148.f, "Down", kUiSmallFont, 10.f, 62.f);
  const Rectangle listBase{panel.x + 16.f, panel.y + 182.f, panel.width - 32.f, 26.f};
  const float dropdownWidth = std::max({textWidth("RotateToVelocity"), textWidth("SpringChain"), textWidth("Add Component")}) + 24.f;

  if (CheckCollisionPointRec(mouse, addButton)) {
    m_componentDropdownOpen = !m_componentDropdownOpen;
    return true;
  }

  if (m_componentDropdownOpen) {
    for (int i = 0; i < 6; ++i) {
        Rectangle row{addButton.x, addButton.y + addButton.height + static_cast<float>(i) * 28.f, dropdownWidth, 26.f};
      if (CheckCollisionPointRec(mouse, row)) {
        addComponent(componentTypeByIndex(i));
        m_componentDropdownOpen = false;
        return true;
      }
    }
  }

  if (CheckCollisionPointRec(mouse, prevButton)) {
    if (!m_components.empty()) {
      m_selectedComponent = (m_selectedComponent + static_cast<int>(m_components.size()) - 1) % static_cast<int>(m_components.size());
      m_activeParamInput = -1;
    }
    m_componentDropdownOpen = false;
    return true;
  }

  if (CheckCollisionPointRec(mouse, nextButton)) {
    if (!m_components.empty()) {
      m_selectedComponent = (m_selectedComponent + 1) % static_cast<int>(m_components.size());
      m_activeParamInput = -1;
    }
    m_componentDropdownOpen = false;
    return true;
  }

  if (CheckCollisionPointRec(mouse, duplicateButton)) {
    duplicateSelectedComponent();
    m_componentDropdownOpen = false;
    return true;
  }

  if (CheckCollisionPointRec(mouse, upButton)) {
    moveSelectedComponent(-1);
    m_componentDropdownOpen = false;
    return true;
  }

  if (CheckCollisionPointRec(mouse, downButton)) {
    moveSelectedComponent(1);
    m_componentDropdownOpen = false;
    return true;
  }

  const int visibleCount = std::min<int>(static_cast<int>(m_components.size()), 5);
  const int listStart = m_components.empty() ? 0 : std::clamp(m_selectedComponent - 2, 0, std::max(0, static_cast<int>(m_components.size()) - visibleCount));
  for (int i = 0; i < visibleCount; ++i) {
    Rectangle row = listBase;
    row.y += 20.f + static_cast<float>(i) * 28.f;
    if (CheckCollisionPointRec(mouse, row)) {
      m_selectedComponent = listStart + i;
      m_activeParamInput = -1;
      m_componentDropdownOpen = false;
      return true;
    }
  }

  if (m_componentDropdownOpen) {
    m_componentDropdownOpen = false;
    return true;
  }

  return false;
}

void AnimationEditorScreen::drawComponentUi(Rectangle panel) const
{
  const Rectangle tabs[] = {
    Rectangle{panel.x + 12.f, panel.y + 42.f, 70.f, 24.f},
    Rectangle{panel.x + 88.f, panel.y + 42.f, 92.f, 24.f},
    Rectangle{panel.x + 186.f, panel.y + 42.f, 62.f, 24.f},
    Rectangle{panel.x + 254.f, panel.y + 42.f, 64.f, 24.f},
  };
  const PanelMode modes[] = {PanelMode::Bone, PanelMode::Component, PanelMode::Render, PanelMode::Export};
  for (size_t i = 0; i < sizeof(tabs) / sizeof(tabs[0]); ++i) {
    const bool selected = m_panelMode == modes[i];
    DrawRectangleRec(tabs[i], selected ? Color{54, 70, 38, 255} : Color{32, 44, 58, 255});
    DrawRectangleLinesEx(tabs[i], 1.f, selected ? GOLD : Color{100, 130, 150, 255});
    DrawText(panelModeName(modes[i]), static_cast<int>(tabs[i].x + 7), static_cast<int>(tabs[i].y + 5), 13, RAYWHITE);
  }

  if (m_panelMode != PanelMode::Component) return;

  const Rectangle addButton = buttonRect(panel.x + 16.f, panel.y + 110.f, "Add Component", kUiSmallFont, 10.f, 136.f);
  const Rectangle prevButton = buttonRect(addButton.x + addButton.width + 10.f, panel.y + 110.f, "<", kUiSmallFont, 10.f, 34.f);
  const Rectangle nextButton = buttonRect(prevButton.x + prevButton.width + 6.f, panel.y + 110.f, ">", kUiSmallFont, 10.f, 34.f);
  const Rectangle duplicateButton = buttonRect(nextButton.x + nextButton.width + 8.f, panel.y + 110.f, "Duplicate", kUiSmallFont, 10.f, 92.f);
  const Rectangle upButton = buttonRect(panel.x + 16.f, panel.y + 148.f, "Up", kUiSmallFont, 10.f, 50.f);
  const Rectangle downButton = buttonRect(upButton.x + upButton.width + 8.f, panel.y + 148.f, "Down", kUiSmallFont, 10.f, 62.f);
  const Rectangle listBase{panel.x + 16.f, panel.y + 182.f, panel.width - 32.f, 26.f};
  const float dropdownWidth = std::max({textWidth("RotateToVelocity"), textWidth("SpringChain"), textWidth("Add Component")}) + 24.f;

  drawButton(addButton, "Add Component", m_componentDropdownOpen);
  drawButton(prevButton, "<");
  drawButton(nextButton, ">");
  drawButton(duplicateButton, "Duplicate");
  drawButton(upButton, "Up");
  drawButton(downButton, "Down");

  if (m_componentDropdownOpen) {
    Rectangle dropdownPanel{addButton.x - 2.f, addButton.y + addButton.height - 1.f, dropdownWidth + 4.f, 6.f * 28.f + 4.f};
    DrawRectangleRec(dropdownPanel, Color{8, 12, 18, 255});
    DrawRectangleLinesEx(dropdownPanel, 1.f, Color{118, 146, 166, 255});
    for (int i = 0; i < 6; ++i) {
      Rectangle row{addButton.x, addButton.y + addButton.height + static_cast<float>(i) * 28.f, dropdownWidth, 26.f};
      DrawRectangleRec(row, Color{10, 14, 20, 255});
      DrawRectangleLinesEx(row, 1.f, Color{118, 146, 166, 255});
      drawUiText(componentName(componentTypeByIndex(i)), row.x + 8.f, row.y + 5.f, RAYWHITE);
    }
  }

  drawUiText("Added components", listBase.x, listBase.y + 2.f, GRAY);
  const int visibleCount = std::min<int>(static_cast<int>(m_components.size()), 5);
  const int listStart = m_components.empty() ? 0 : std::clamp(m_selectedComponent - 2, 0, std::max(0, static_cast<int>(m_components.size()) - visibleCount));
  for (int i = 0; i < visibleCount; ++i) {
    const int componentIndex = listStart + i;
    Rectangle row = listBase;
    row.y += 22.f + static_cast<float>(i) * 28.f;
    const bool selected = componentIndex == m_selectedComponent;
    DrawRectangleRec(row, selected ? Color{58, 72, 40, 255} : Color{18, 24, 32, 255});
    DrawRectangleLinesEx(row, 1.f, selected ? GOLD : Color{92, 122, 144, 255});
    drawUiText(TextFormat("%d  %s  %s", componentIndex, componentName(m_components[componentIndex].type), m_components[componentIndex].enabled ? "on" : "off"), row.x + 8.f, row.y + 5.f, RAYWHITE);
  }
}

bool AnimationEditorScreen::handleRenderUi(Vector2 mouse, Rectangle panel)
{
  if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || m_panelMode != PanelMode::Render) return false;

  const Rectangle addButton = buttonRect(panel.x + 16.f, panel.y + 110.f, "Add Render Shape", kUiSmallFont, 10.f, 158.f);
  const Rectangle prevButton = buttonRect(addButton.x + addButton.width + 10.f, panel.y + 110.f, "<", kUiSmallFont, 10.f, 34.f);
  const Rectangle nextButton = buttonRect(prevButton.x + prevButton.width + 6.f, panel.y + 110.f, ">", kUiSmallFont, 10.f, 34.f);
  const Rectangle listBase{panel.x + 16.f, panel.y + 148.f, panel.width - 32.f, 26.f};
  const float dropdownWidth = std::max({textWidth("Add Render Shape"), textWidth("CircleBody"), textWidth("SpineBody")}) + 24.f;

  if (CheckCollisionPointRec(mouse, addButton)) {
    m_renderDropdownOpen = !m_renderDropdownOpen;
    return true;
  }

  if (CheckCollisionPointRec(mouse, prevButton)) {
    if (!m_renderShapes.empty()) {
      m_selectedRenderShape = (m_selectedRenderShape + static_cast<int>(m_renderShapes.size()) - 1) % static_cast<int>(m_renderShapes.size());
      m_activeParamInput = -1;
    }
    m_renderDropdownOpen = false;
    return true;
  }

  if (CheckCollisionPointRec(mouse, nextButton)) {
    if (!m_renderShapes.empty()) {
      m_selectedRenderShape = (m_selectedRenderShape + 1) % static_cast<int>(m_renderShapes.size());
      m_activeParamInput = -1;
    }
    m_renderDropdownOpen = false;
    return true;
  }

  if (m_renderDropdownOpen) {
    for (int i = 0; i < 3; ++i) {
        Rectangle row{addButton.x, addButton.y + addButton.height + static_cast<float>(i) * 28.f, dropdownWidth, 26.f};
      if (CheckCollisionPointRec(mouse, row)) {
        addRenderShape(renderTypeByIndex(i));
        m_renderDropdownOpen = false;
        return true;
      }
    }
  }

  const int visibleCount = std::min<int>(static_cast<int>(m_renderShapes.size()), 5);
  const int listStart = m_renderShapes.empty() ? 0 : std::clamp(m_selectedRenderShape - 2, 0, std::max(0, static_cast<int>(m_renderShapes.size()) - visibleCount));
  for (int i = 0; i < visibleCount; ++i) {
    Rectangle row = listBase;
    row.y += 22.f + static_cast<float>(i) * 28.f;
    if (CheckCollisionPointRec(mouse, row)) {
      m_selectedRenderShape = listStart + i;
      m_activeParamInput = -1;
      m_renderDropdownOpen = false;
      return true;
    }
  }

  if (m_renderDropdownOpen) {
    m_renderDropdownOpen = false;
    return true;
  }

  return false;
}

void AnimationEditorScreen::drawRenderUi(Rectangle panel) const
{
  if (m_panelMode != PanelMode::Render) return;

  const Rectangle addButton = buttonRect(panel.x + 16.f, panel.y + 110.f, "Add Render Shape", kUiSmallFont, 10.f, 158.f);
  const Rectangle prevButton = buttonRect(addButton.x + addButton.width + 10.f, panel.y + 110.f, "<", kUiSmallFont, 10.f, 34.f);
  const Rectangle nextButton = buttonRect(prevButton.x + prevButton.width + 6.f, panel.y + 110.f, ">", kUiSmallFont, 10.f, 34.f);
  const Rectangle listBase{panel.x + 16.f, panel.y + 148.f, panel.width - 32.f, 26.f};
  const float dropdownWidth = std::max({textWidth("Add Render Shape"), textWidth("CircleBody"), textWidth("SpineBody")}) + 24.f;

  drawButton(addButton, "Add Render Shape", m_renderDropdownOpen);
  drawButton(prevButton, "<");
  drawButton(nextButton, ">");

  if (m_renderDropdownOpen) {
    Rectangle dropdownPanel{addButton.x - 2.f, addButton.y + addButton.height - 1.f, dropdownWidth + 4.f, 3.f * 28.f + 4.f};
    DrawRectangleRec(dropdownPanel, Color{8, 12, 18, 255});
    DrawRectangleLinesEx(dropdownPanel, 1.f, Color{118, 146, 166, 255});
    for (int i = 0; i < 3; ++i) {
      Rectangle row{addButton.x, addButton.y + addButton.height + static_cast<float>(i) * 28.f, dropdownWidth, 26.f};
      DrawRectangleRec(row, Color{10, 14, 20, 255});
      DrawRectangleLinesEx(row, 1.f, Color{118, 146, 166, 255});
      drawUiText(renderName(renderTypeByIndex(i)), row.x + 8.f, row.y + 5.f, RAYWHITE);
    }
  }

  drawUiText("Added render shapes", listBase.x, listBase.y + 2.f, GRAY);
  const int visibleCount = std::min<int>(static_cast<int>(m_renderShapes.size()), 5);
  const int listStart = m_renderShapes.empty() ? 0 : std::clamp(m_selectedRenderShape - 2, 0, std::max(0, static_cast<int>(m_renderShapes.size()) - visibleCount));
  for (int i = 0; i < visibleCount; ++i) {
    const int renderIndex = listStart + i;
    Rectangle row = listBase;
    row.y += 22.f + static_cast<float>(i) * 28.f;
    const bool selected = renderIndex == m_selectedRenderShape;
    DrawRectangleRec(row, selected ? Color{58, 72, 40, 255} : Color{18, 24, 32, 255});
    DrawRectangleLinesEx(row, 1.f, selected ? GOLD : Color{92, 122, 144, 255});
    drawUiText(TextFormat("%d  %s  %s", renderIndex, renderName(m_renderShapes[renderIndex].type), m_renderShapes[renderIndex].enabled ? "on" : "off"), row.x + 8.f, row.y + 5.f, RAYWHITE);
  }
}

void AnimationEditorScreen::cycleColorPreset()
{
  if (m_selectedRenderShape < 0 || m_selectedRenderShape >= static_cast<int>(m_renderShapes.size())) return;
  static const Color colors[] = {
    Color{80, 220, 210, 190},
    Color{230, 110, 80, 210},
    Color{120, 80, 230, 200},
    Color{70, 220, 120, 200},
    Color{210, 70, 240, 200},
  };
  EditableRenderShape& shape = m_renderShapes[m_selectedRenderShape];
  int index = 0;
  for (size_t i = 0; i < sizeof(colors) / sizeof(colors[0]); ++i) {
    if (shape.color.r == colors[i].r && shape.color.g == colors[i].g && shape.color.b == colors[i].b) index = static_cast<int>(i + 1);
  }
  pushUndoSnapshot();
  shape.color = colors[index % (sizeof(colors) / sizeof(colors[0]))];
  m_dirtyPreview = true;
}

void AnimationEditorScreen::saveRig()
{
  json root;
  for (const EditableBone& bone : m_bones) {
    root["bones"].push_back({
      {"name", bone.name}, {"parent", bone.parent},
      {"x", bone.bindLocal.position.x}, {"y", bone.bindLocal.position.y},
      {"rotation", bone.bindLocal.rotation}
    });
  }
  for (const EditableComponent& c : m_components) {
    root["components"].push_back({
      {"type", componentTypeToString(c.type)}, {"enabled", c.enabled}, {"start", c.bones.start}, {"count", c.bones.count}, {"bone", c.bone},
      {"idleAmplitude", c.idleAmplitude}, {"idleFrequency", c.idleFrequency}, {"moveAmplitude", c.moveAmplitude}, {"moveFrequency", c.moveFrequency},
      {"phaseOffset", c.phaseOffset}, {"maxAngle", c.maxAngle}, {"distribution", c.distribution}, {"stiffness", c.stiffness}, {"damping", c.damping},
      {"amplitude", c.amplitude}, {"frequency", c.frequency}, {"turnSpeed", c.turnSpeed}, {"lookWeight", c.lookWeight}, {"weight", c.weight}
    });
  }
  for (const EditableRenderShape& r : m_renderShapes) {
    root["renders"].push_back({
      {"type", renderTypeToString(r.type)}, {"enabled", r.enabled}, {"start", r.bones.start}, {"count", r.bones.count}, {"bone", r.bone},
      {"width", r.width}, {"radius", r.radius}, {"color", colorToJson(r.color)}, {"outline", colorToJson(r.outlineColor)}
    });
  }
  std::ofstream out(kRigPath);
  out << root.dump(2);
  m_statusText = std::string("Saved ") + kRigPath;
}

void AnimationEditorScreen::loadRig()
{
  std::ifstream in(kRigPath);
  if (!in) {
    m_statusText = std::string("Missing ") + kRigPath;
    return;
  }
  json root;
  in >> root;
  pushUndoSnapshot();

  m_bones.clear();
  for (const json& item : root.value("bones", json::array())) {
    EditableBone bone;
    bone.name = item.value("name", "bone");
    bone.parent = item.value("parent", -1);
    bone.bindLocal.position = {item.value("x", 0.f), item.value("y", 0.f)};
    bone.bindLocal.rotation = item.value("rotation", 0.f);
    bone.bindLocal.scale = {1.f, 1.f};
    m_bones.push_back(bone);
  }
  int normalizedParents = 0;
  for (size_t i = 0; i < m_bones.size(); ++i) {
    if (!canReparentBone(static_cast<int>(i), m_bones[i].parent)) {
      m_bones[i].parent = -1;
      ++normalizedParents;
    }
  }

  m_components.clear();
  for (const json& item : root.value("components", json::array())) {
    EditableComponent c;
    c.type = componentTypeFromJson(item.value("type", json(3)), AnimationEditorScreen::ComponentType::WaveChain);
    c.enabled = item.value("enabled", true);
    c.bones = {static_cast<uint16_t>(item.value("start", 0)), static_cast<uint16_t>(item.value("count", 1))};
    c.bone = static_cast<uint16_t>(item.value("bone", 0));
    c.idleAmplitude = item.value("idleAmplitude", c.idleAmplitude);
    c.idleFrequency = item.value("idleFrequency", c.idleFrequency);
    c.moveAmplitude = item.value("moveAmplitude", c.moveAmplitude);
    c.moveFrequency = item.value("moveFrequency", c.moveFrequency);
    c.phaseOffset = item.value("phaseOffset", c.phaseOffset);
    c.maxAngle = item.value("maxAngle", c.maxAngle);
    c.distribution = item.value("distribution", c.distribution);
    c.stiffness = item.value("stiffness", c.stiffness);
    c.damping = item.value("damping", c.damping);
    c.amplitude = item.value("amplitude", c.amplitude);
    c.frequency = item.value("frequency", c.frequency);
    c.turnSpeed = item.value("turnSpeed", c.turnSpeed);
    c.lookWeight = item.value("lookWeight", c.lookWeight);
    c.weight = item.value("weight", c.weight);
    m_components.push_back(c);
  }

  m_renderShapes.clear();
  for (const json& item : root.value("renders", json::array())) {
    EditableRenderShape r;
    r.type = renderTypeFromJson(item.value("type", json(0)), AnimationEditorScreen::RenderType::SpineBody);
    r.enabled = item.value("enabled", true);
    r.bones = {static_cast<uint16_t>(item.value("start", 0)), static_cast<uint16_t>(item.value("count", 1))};
    r.bone = static_cast<uint16_t>(item.value("bone", 0));
    r.width = item.value("width", r.width);
    r.radius = item.value("radius", r.radius);
    r.color = colorFromJson(item.value("color", json::array()), r.color);
    r.outlineColor = colorFromJson(item.value("outline", json::array()), r.outlineColor);
    m_renderShapes.push_back(r);
  }

  m_selectedBone = m_bones.empty() ? -1 : 0;
  m_selectedComponent = m_components.empty() ? -1 : 0;
  m_selectedRenderShape = m_renderShapes.empty() ? -1 : 0;
  m_activeParamInput = -1;
  syncEditableRanges();
  m_dirtyPreview = true;
  m_statusText = std::string("Loaded ") + kRigPath;
  if (normalizedParents > 0) m_statusText += TextFormat("; reset %d invalid parent(s)", normalizedParents);
}

void AnimationEditorScreen::exportFactory() const
{
  std::ofstream out(kExportPath);
  out << "void createEditedRig(AnimationWorld& world) {\n";
  out << "  const uint32_t skeletonId = static_cast<uint32_t>(world.skeletons.size());\n";
  out << "  Skeleton skeleton;\n";
  for (const EditableBone& bone : m_bones) {
    out << "  skeleton.bones.push_back(Bone{" << bone.parent << ", Transform2D{{" << bone.bindLocal.position.x << "f, " << bone.bindLocal.position.y << "f}, " << bone.bindLocal.rotation << "f, {1.f, 1.f}}});\n";
  }
  out << "  world.skeletons.push_back(skeleton);\n";
  out << "  AnimationInstance instance;\n  instance.skeletonId = skeletonId;\n  instance.transform = Transform2D{{GetScreenWidth() / 2.f, GetScreenHeight() / 2.f}, 0.f, {1.f, 1.f}};\n  instance.moveAmount = 1.f;\n";
  out << "  const uint32_t instanceId = static_cast<uint32_t>(world.instances.size());\n  world.instances.push_back(instance);\n";
  for (const EditableComponent& c : m_components) {
    if (!c.enabled) continue;
    out << "  // " << componentName(c.type) << "\n";
    if (c.type == ComponentType::WaveChain) out << "  addWaveChain(world, instanceId, WaveChainParams{{" << c.bones.start << ", " << c.bones.count << "}, " << c.idleAmplitude << "f, " << c.idleFrequency << "f, " << c.moveAmplitude << "f, " << c.moveFrequency << "f, " << c.phaseOffset << "f}, " << c.weight << "f);\n";
    if (c.type == ComponentType::BendChain) out << "  addBendChain(world, instanceId, BendChainParams{{" << c.bones.start << ", " << c.bones.count << "}, " << c.maxAngle << "f, " << c.distribution << "f}, " << c.weight << "f);\n";
    if (c.type == ComponentType::SpringChain) out << "  addSpringChain(world, instanceId, SpringChainParams{{" << c.bones.start << ", " << c.bones.count << "}, " << c.stiffness << "f, " << c.damping << "f}, " << c.weight << "f);\n";
    if (c.type == ComponentType::Pulse) out << "  addPulse(world, instanceId, PulseParams{{" << c.bones.start << ", " << c.bones.count << "}, " << c.amplitude << "f, " << c.frequency << "f, " << c.phaseOffset << "f}, " << c.weight << "f);\n";
    if (c.type == ComponentType::LookAt) out << "  addLookAt(world, instanceId, LookAtParams{" << c.bone << ", {}, " << c.lookWeight << "f}, " << c.weight << "f);\n";
    if (c.type == ComponentType::RotateToVelocity) out << "  addRotateToVelocity(world, instanceId, RotateToVelocityParams{" << c.turnSpeed << "f}, " << c.weight << "f);\n";
  }
  for (const EditableRenderShape& r : m_renderShapes) {
    if (!r.enabled) continue;
    out << "  // " << renderName(r.type) << "\n";
    if (r.type == RenderType::SpineBody) {
      out << "  SpineBodyRenderParams spineBody;\n";
      out << "  spineBody.bones = {" << r.bones.start << ", " << r.bones.count << "};\n";
      out << "  spineBody.color = Color{" << static_cast<int>(r.color.r) << ", " << static_cast<int>(r.color.g) << ", " << static_cast<int>(r.color.b) << ", " << static_cast<int>(r.color.a) << "};\n";
      out << "  spineBody.outlineColor = Color{" << static_cast<int>(r.outlineColor.r) << ", " << static_cast<int>(r.outlineColor.g) << ", " << static_cast<int>(r.outlineColor.b) << ", " << static_cast<int>(r.outlineColor.a) << "};\n";
      out << "  spineBody.widths.assign(spineBody.bones.count, " << r.width << "f);\n";
      out << "  world.instances[instanceId].hasSpineBody = true;\n";
      out << "  world.instances[instanceId].spineBodyId = addSpineBody(world, spineBody);\n";
    }
    if (r.type == RenderType::Appendage) {
      out << "  AppendageRenderParams appendage;\n";
      out << "  appendage.bones = {" << r.bones.start << ", " << r.bones.count << "};\n";
      out << "  appendage.color = Color{" << static_cast<int>(r.color.r) << ", " << static_cast<int>(r.color.g) << ", " << static_cast<int>(r.color.b) << ", " << static_cast<int>(r.color.a) << "};\n";
      out << "  appendage.outlineColor = Color{" << static_cast<int>(r.outlineColor.r) << ", " << static_cast<int>(r.outlineColor.g) << ", " << static_cast<int>(r.outlineColor.b) << ", " << static_cast<int>(r.outlineColor.a) << "};\n";
      out << "  appendage.widths.assign(appendage.bones.count, " << r.width << "f);\n";
      out << "  world.instances[instanceId].appendageIds.push_back(addAppendage(world, appendage));\n";
    }
    if (r.type == RenderType::CircleBody) {
      out << "  CircleBodyRenderParams circleBody;\n";
      out << "  circleBody.bone = " << r.bone << ";\n";
      out << "  circleBody.radius = " << r.radius << "f;\n";
      out << "  circleBody.color = Color{" << static_cast<int>(r.color.r) << ", " << static_cast<int>(r.color.g) << ", " << static_cast<int>(r.color.b) << ", " << static_cast<int>(r.color.a) << "};\n";
      out << "  circleBody.outlineColor = Color{" << static_cast<int>(r.outlineColor.r) << ", " << static_cast<int>(r.outlineColor.g) << ", " << static_cast<int>(r.outlineColor.b) << ", " << static_cast<int>(r.outlineColor.a) << "};\n";
      out << "  world.instances[instanceId].hasCircleBody = true;\n";
      out << "  world.instances[instanceId].circleBodyId = addCircleBody(world, circleBody);\n";
    }
  }
  out << "}\n";
}

void AnimationEditorScreen::Update()
{
  const float dt = GetFrameTime();
  const Vector2 mouse = GetMousePosition();
  const Rectangle leftPanel{16.f, 16.f, kLeftPanelWidth, static_cast<float>(GetScreenHeight() - 32)};
  const Rectangle rightPanel{static_cast<float>(GetScreenWidth() - (kRightPanelWidth + 16.f)), 16.f, kRightPanelWidth, static_cast<float>(GetScreenHeight() - 32)};
  const bool uiCaptured = CheckCollisionPointRec(mouse, leftPanel) || CheckCollisionPointRec(mouse, rightPanel);
  layoutParamInputs(rightPanel);
  updateParamInputs(mouse);
  const bool clickedComponentUi = handleComponentUi(mouse, rightPanel);
  const bool clickedRenderUi = handleRenderUi(mouse, rightPanel);

  if (IsKeyPressed(KEY_ESCAPE) && !isEditingText()) {
    m_finishScreen = Screen::GameScreen::ANIMATION_TEST;
    return;
  }
  if (isEditingText()) return;

  const bool ctrlDown = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
  const bool altDown = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
  const bool shiftDown = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
  if (ctrlDown && IsKeyPressed(KEY_Z)) undo();
  if (ctrlDown && IsKeyPressed(KEY_Y)) redo();

  if (IsKeyPressed(KEY_P)) m_previewPaused = !m_previewPaused;
  if (IsKeyPressed(KEY_M)) { m_panelMode = static_cast<PanelMode>((static_cast<int>(m_panelMode) + 1) % 4); m_activeParamInput = -1; m_componentDropdownOpen = false; m_renderDropdownOpen = false; }
  if (IsKeyPressed(KEY_G)) { pushUndoSnapshot(); m_snapToGrid = !m_snapToGrid; }
  if (IsKeyPressed(KEY_I)) importSpineSample();
  if (ctrlDown && IsKeyPressed(KEY_S)) saveRig();
  if (ctrlDown && IsKeyPressed(KEY_L)) loadRig();
  if (ctrlDown && IsKeyPressed(KEY_E)) { exportFactory(); m_statusText = std::string("Exported ") + kExportPath; }

  if (IsKeyPressed(KEY_ONE)) addComponent(ComponentType::RotateToVelocity);
  if (IsKeyPressed(KEY_TWO)) addComponent(ComponentType::LookAt);
  if (IsKeyPressed(KEY_THREE)) addComponent(ComponentType::BendChain);
  if (IsKeyPressed(KEY_FOUR)) addComponent(ComponentType::WaveChain);
  if (IsKeyPressed(KEY_FIVE)) addComponent(ComponentType::Pulse);
  if (IsKeyPressed(KEY_SIX)) addComponent(ComponentType::SpringChain);
  if (IsKeyPressed(KEY_SEVEN)) addRenderShape(RenderType::SpineBody);
  if (IsKeyPressed(KEY_EIGHT)) addRenderShape(RenderType::Appendage);
  if (IsKeyPressed(KEY_NINE)) addRenderShape(RenderType::CircleBody);
  if (IsKeyPressed(KEY_TAB) && !m_components.empty()) { m_selectedComponent = (m_selectedComponent + 1) % m_components.size(); m_panelMode = PanelMode::Component; m_activeParamInput = -1; }
  if (IsKeyPressed(KEY_V) && !m_renderShapes.empty()) { m_selectedRenderShape = (m_selectedRenderShape + 1) % m_renderShapes.size(); m_panelMode = PanelMode::Render; m_activeParamInput = -1; }
  if (IsKeyPressed(KEY_C) && m_selectedComponent >= 0 && m_selectedComponent < static_cast<int>(m_components.size())) { pushUndoSnapshot(); m_components[m_selectedComponent].enabled = !m_components[m_selectedComponent].enabled; m_dirtyPreview = true; }
  if (IsKeyPressed(KEY_B) && m_selectedRenderShape >= 0 && m_selectedRenderShape < static_cast<int>(m_renderShapes.size())) { pushUndoSnapshot(); m_renderShapes[m_selectedRenderShape].enabled = !m_renderShapes[m_selectedRenderShape].enabled; m_dirtyPreview = true; }
  if (IsKeyPressed(KEY_K)) cycleColorPreset();
  if (IsKeyPressed(KEY_X)) {
    if (m_panelMode == PanelMode::Component) deleteSelectedComponent();
    else if (m_panelMode == PanelMode::Render) deleteSelectedRenderShape();
  }
  if (m_panelMode == PanelMode::Component && ctrlDown && IsKeyPressed(KEY_D)) duplicateSelectedComponent();
  if (m_panelMode == PanelMode::Component && altDown && IsKeyPressed(KEY_UP)) moveSelectedComponent(-1);
  if (m_panelMode == PanelMode::Component && altDown && IsKeyPressed(KEY_DOWN)) moveSelectedComponent(1);
  if (!ctrlDown && IsKeyPressed(KEY_Y)) duplicateSelectedBone();
  if (IsKeyPressed(KEY_DELETE) || IsKeyPressed(KEY_BACKSPACE)) {
    if (m_panelMode == PanelMode::Bone) {
      if (shiftDown) deleteSelectedBone();
      else m_statusText = "Use Shift+Delete to delete bone subtree";
    } else if (m_panelMode == PanelMode::Component) {
      deleteSelectedComponent();
    } else if (m_panelMode == PanelMode::Render) {
      deleteSelectedRenderShape();
    }
  }

  ensurePreviewPose();
  m_hoveredBone = uiCaptured ? -1 : pickBone(mouse);

  bool clickedInput = uiCaptured || clickedComponentUi || clickedRenderUi;
  for (const ParamInput& input : m_paramInputs) clickedInput = clickedInput || CheckCollisionPointRec(mouse, input.bounds);
  if (!clickedInput && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) createBone(mouse);
  if (!clickedInput && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    m_selectedBone = m_hoveredBone;
    m_draggingBone = m_selectedBone >= 0;
    if (m_draggingBone) pushUndoSnapshot();
  }
  if (!uiCaptured && m_draggingBone && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && m_selectedBone >= 0 && m_selectedBone < static_cast<int>(m_bones.size())) {
    m_bones[m_selectedBone].bindLocal.position = parentLocalPosition(m_bones[m_selectedBone].parent, mouse);
    m_dirtyPreview = true;
  }
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) m_draggingBone = false;

  if (!m_previewPaused) {
    ensurePreviewPose();
    if (!m_previewWorld.instances.empty()) updateAnimationWorld(m_previewWorld, dt);
  }
}

void AnimationEditorScreen::Paint()
{
  ClearBackground(Color{8, 12, 18, 255});
  const Rectangle leftPanel{16.f, 16.f, kLeftPanelWidth, static_cast<float>(GetScreenHeight() - 32)};
  const Rectangle rightPanel{static_cast<float>(GetScreenWidth() - (kRightPanelWidth + 16.f)), 16.f, kRightPanelWidth, static_cast<float>(GetScreenHeight() - 32)};
  layoutParamInputs(rightPanel);
  drawPanel(leftPanel, "Animation Rig Editor");
  drawPanel(rightPanel, panelModeName(m_panelMode));

  for (int x = 360; x < GetScreenWidth() - 360; x += 40) DrawLine(x, 0, x, GetScreenHeight(), Color{28, 36, 46, 255});
  for (int y = 0; y < GetScreenHeight(); y += 40) DrawLine(340, y, GetScreenWidth() - 350, y, Color{28, 36, 46, 255});

  ensurePreviewPose();
  if (!m_previewWorld.instances.empty()) {
    const AnimationInstance& instance = m_previewWorld.instances[0];
    if (instance.hasSpineBody) drawSpineBody(m_previewWorld.spineBodies[instance.spineBodyId], instance.pose);
    for (uint32_t id : instance.appendageIds) drawAppendage(m_previewWorld.appendages[id], instance.pose);
    if (instance.hasCircleBody) drawCircleBody(m_previewWorld.circleBodies[instance.circleBodyId], instance.pose);
    drawSkeleton(m_previewWorld.skeletons[0], instance.pose);
    for (size_t i = 0; i < instance.pose.world.size(); ++i) {
      const Vector2 p = instance.pose.world[i].position;
      const Color color = static_cast<int>(i) == m_selectedBone ? YELLOW : (static_cast<int>(i) == m_hoveredBone ? ORANGE : SKYBLUE);
      DrawCircleV(p, 7.f, color);
      drawUiText(TextFormat("%zu", i), p.x + 9.f, p.y - 8.f, RAYWHITE, kUiSmallFont);
    }
  }

  float y = 54.f;
  drawUiText("Dev editor", 32.f, y, RAYWHITE, kUiFont); y += 30.f;
  drawUiText("F2 opens | Esc returns", 32.f, y, GRAY); y += 26.f;
  y = drawWrappedUiText("Mouse: select or drag bones on the canvas. Right-click creates a child bone from the current selection.", 32.f, y, leftPanel.width - 32.f, LIGHTGRAY);
  y = drawWrappedUiText("Shift+Delete removes the selected bone subtree. Y duplicates the selected bone.", 32.f, y, leftPanel.width - 32.f, LIGHTGRAY);
  y = drawWrappedUiText("M cycles panels. G toggles snap. P pauses preview. I imports the sample rig.", 32.f, y, leftPanel.width - 32.f, LIGHTGRAY);
  y = drawWrappedUiText("1-6 add procedures. 7-9 add render shapes. Tab and V cycle current component or render selections.", 32.f, y, leftPanel.width - 32.f, LIGHTGRAY);
  y = drawWrappedUiText("C/B toggle enabled state. Delete or X is context-sensitive. Ctrl+Z/Y undo redo. Ctrl+S save. Ctrl+L load. Ctrl+E export.", 32.f, y, leftPanel.width - 32.f, LIGHTGRAY);
  y += 8.f;
  drawUiText(TextFormat("Bones: %zu  Selected: %d", m_bones.size(), m_selectedBone), 32.f, y, RAYWHITE, kUiFont); y += 24.f;
  drawUiText(TextFormat("Components: %zu  Renders: %zu", m_components.size(), m_renderShapes.size()), 32.f, y, RAYWHITE, kUiFont); y += 24.f;
  drawUiText(TextFormat("Preview: %s  Snap: %s", m_previewPaused ? "paused" : "running", m_snapToGrid ? "on" : "off"), 32.f, y, RAYWHITE, kUiFont); y += 30.f;
  if (!m_statusText.empty()) {
    drawWrappedUiText(m_statusText.c_str(), 32.f, y, leftPanel.width - 32.f, GOLD);
  }

  y = static_cast<int>(rightPanel.y + 74.f);
  drawComponentUi(rightPanel);
  drawRenderUi(rightPanel);
  if (m_panelMode == PanelMode::Bone) {
    DrawText("Selected bone", static_cast<int>(rightPanel.x + 16), y, 16, RAYWHITE); y += 24;
    if (m_selectedBone >= 0 && m_selectedBone < static_cast<int>(m_bones.size())) {
      const EditableBone& bone = m_bones[m_selectedBone];
      DrawText(TextFormat("%d: %s", m_selectedBone, bone.name.c_str()), static_cast<int>(rightPanel.x + 16), y, 15, LIGHTGRAY); y += 20;
      DrawText(TextFormat("Parent: %d", bone.parent), static_cast<int>(rightPanel.x + 16), y, 14, GRAY);
    } else {
      DrawText("No bone selected", static_cast<int>(rightPanel.x + 16), y, 15, LIGHTGRAY);
    }
  } else if (m_panelMode == PanelMode::Component) {
    DrawText("Selected procedure", static_cast<int>(rightPanel.x + 16), y, 16, RAYWHITE); y += 24;
    if (m_selectedComponent >= 0 && m_selectedComponent < static_cast<int>(m_components.size())) {
      const EditableComponent& c = m_components[m_selectedComponent];
      DrawText(TextFormat("%d: %s %s", m_selectedComponent, componentName(c.type), c.enabled ? "on" : "off"), static_cast<int>(rightPanel.x + 16), y, 15, LIGHTGRAY);
    } else {
      DrawText("No component selected", static_cast<int>(rightPanel.x + 16), y, 15, LIGHTGRAY);
    }
  } else if (m_panelMode == PanelMode::Render) {
    DrawText("Selected render shape", static_cast<int>(rightPanel.x + 16), y, 16, RAYWHITE); y += 24;
    if (m_selectedRenderShape >= 0 && m_selectedRenderShape < static_cast<int>(m_renderShapes.size())) {
      const EditableRenderShape& r = m_renderShapes[m_selectedRenderShape];
      DrawText(TextFormat("%d: %s %s", m_selectedRenderShape, renderName(r.type), r.enabled ? "on" : "off"), static_cast<int>(rightPanel.x + 16), y, 15, LIGHTGRAY);
    } else {
      DrawText("No render selected", static_cast<int>(rightPanel.x + 16), y, 15, LIGHTGRAY);
    }
  } else {
    DrawText("Save/load/export", static_cast<int>(rightPanel.x + 16), y, 16, RAYWHITE); y += 28;
    DrawText(kRigPath, static_cast<int>(rightPanel.x + 16), y, 14, LIGHTGRAY); y += 22;
    DrawText(kExportPath, static_cast<int>(rightPanel.x + 16), y, 14, LIGHTGRAY);
  }
  if (m_panelMode != PanelMode::Export) {
    const float hintY = rightPanel.y + ((m_panelMode == PanelMode::Component || m_panelMode == PanelMode::Render) ? 286.f : 166.f);
    drawUiText("Click field, type, Enter applies", rightPanel.x + 16.f, hintY, GRAY);
  }
  drawParamInputs();
}

Screen::GameScreen AnimationEditorScreen::Finish()
{
  return m_finishScreen;
}

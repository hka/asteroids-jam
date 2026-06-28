#include "ui/ui_layout.h"

#include <algorithm>
#include <sstream>

namespace ui {

namespace {

Font resolveFont(const Font* font)
{
  return font != nullptr ? *font : GetFontDefault();
}

float textSpacing()
{
  return 1.f;
}

float mainAxisSize(const Rectangle& rect, Axis axis)
{
  return axis == Axis::Horizontal ? rect.width : rect.height;
}

float crossAxisSize(const Rectangle& rect, Axis axis)
{
  return axis == Axis::Horizontal ? rect.height : rect.width;
}

Rectangle shrink(Rectangle rect, Edges padding)
{
  rect.x += padding.left;
  rect.y += padding.top;
  rect.width = std::max(0.f, rect.width - padding.left - padding.right);
  rect.height = std::max(0.f, rect.height - padding.top - padding.bottom);
  return rect;
}

bool colorsEqual(Color a, Color b)
{
  return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

bool edgesEqual(Edges a, Edges b)
{
  return a.left == b.left && a.top == b.top && a.right == b.right && a.bottom == b.bottom;
}

bool sizesEqual(Size a, Size b)
{
  return a.mode == b.mode && a.value == b.value;
}

bool layoutsEqual(const Layout& a, const Layout& b)
{
  return a.axis == b.axis &&
         sizesEqual(a.width, b.width) &&
         sizesEqual(a.height, b.height) &&
         a.crossAlign == b.crossAlign &&
         edgesEqual(a.padding, b.padding) &&
         a.gap == b.gap;
}

bool stylesEqual(const Style& a, const Style& b)
{
  return colorsEqual(a.background, b.background) &&
         colorsEqual(a.border, b.border) &&
         colorsEqual(a.text, b.text) &&
         a.borderThickness == b.borderThickness &&
         a.fontSize == b.fontSize;
}

} // namespace

Size Size::pixels(float value)
{
  return Size{SizeMode::Pixels, value};
}

Size Size::grow(float weight)
{
  return Size{SizeMode::Grow, weight};
}

Size Size::fit()
{
  return Size{SizeMode::Fit, 0.f};
}

Size Size::percent(float value)
{
  return Size{SizeMode::Percent, value};
}

Edges Edges::all(float value)
{
  return Edges{value, value, value, value};
}

Edges Edges::symmetric(float horizontal, float vertical)
{
  return Edges{horizontal, vertical, horizontal, vertical};
}

void Context::clear()
{
  m_nodes.clear();
  m_nodeLookup.clear();
  m_items.clear();
  m_interaction = {};
}

void Context::beginFrame(Rectangle rootBounds, Vector2 mouse, bool mousePressed, bool mouseDown, bool mouseReleased)
{
  m_rootBounds = rootBounds;
  m_mouse = mouse;
  m_mousePressed = mousePressed;
  m_mouseDown = mouseDown;
  m_mouseReleased = mouseReleased;
  m_interaction = {};
  ensureRoot();

  Node& root = m_nodes[0];
  if (root.bounds.x != rootBounds.x || root.bounds.y != rootBounds.y || root.bounds.width != rootBounds.width || root.bounds.height != rootBounds.height) {
    root.bounds = rootBounds;
    markLayoutDirty(0);
  }
}

void Context::setRootLayout(const Layout& layout)
{
  ensureRoot();
  if (!layoutsEqual(m_nodes[0].layout, layout)) {
    m_nodes[0].layout = layout;
    markLayoutDirty(0);
  }
}

void Context::upsertNode(Id id, Id parentId, NodeKind kind, const Layout& layout, const Style& style, const char* text)
{
  if (id == kNoId) return;
  ensureRoot();

  const int parentIndex = parentId == kNoId ? 0 : findNodeIndex(parentId);
  if (parentIndex < 0) return;

  const std::string newText = text != nullptr ? text : "";
  const int existingIndex = findNodeIndex(id);
  if (existingIndex >= 0) {
    Node& node = m_nodes[existingIndex];
    if (node.parent != parentIndex) {
      if (node.parent >= 0) detachChild(node.parent, existingIndex);
      node.parent = parentIndex;
      attachChild(parentIndex, existingIndex);
      markLayoutDirty(parentIndex);
    }
    if (node.kind != kind) {
      node.kind = kind;
      markLayoutDirty(existingIndex);
    }
    setLayout(id, layout);
    setStyle(id, style);
    setText(id, newText.c_str());
    return;
  }

  Node node;
  node.id = id;
  node.parent = parentIndex;
  node.kind = kind;
  node.layout = layout;
  node.style = style;
  node.text = newText;
  const int nodeIndex = static_cast<int>(m_nodes.size());
  m_nodes.push_back(node);
  m_nodeLookup[id] = nodeIndex;
  attachChild(parentIndex, nodeIndex);
  markLayoutDirty(parentIndex);
}

void Context::setLayout(Id id, const Layout& layout)
{
  const int nodeIndex = findNodeIndex(id);
  if (nodeIndex < 0) return;
  Node& node = m_nodes[nodeIndex];
  if (layoutsEqual(node.layout, layout)) return;
  node.layout = layout;
  markLayoutDirty(nodeIndex);
}

void Context::setStyle(Id id, const Style& style)
{
  const int nodeIndex = findNodeIndex(id);
  if (nodeIndex < 0) return;
  Node& node = m_nodes[nodeIndex];
  if (stylesEqual(node.style, style)) return;
  const Style previous = node.style;
  node.style = style;
  if (styleAffectsLayout(node, previous, style)) markLayoutDirty(nodeIndex);
}

void Context::setText(Id id, const char* text)
{
  const int nodeIndex = findNodeIndex(id);
  if (nodeIndex < 0) return;
  Node& node = m_nodes[nodeIndex];
  const std::string newText = text != nullptr ? text : "";
  if (node.text == newText) return;
  node.text = newText;
  if (textAffectsLayout(node)) markLayoutDirty(nodeIndex);
}

void Context::compute()
{
  if (m_nodes.empty()) return;
  if (m_nodes[0].layoutDirty || m_nodes[0].subtreeDirty) layoutNode(0, m_rootBounds, m_nodes[0].layoutDirty);
  collectItems();
  updateInteraction();
}

void Context::draw() const
{
  const Font font = resolveFont(m_font);
  for (const Item& item : m_items) {
    if (item.kind != NodeKind::Label && item.kind != NodeKind::WrappedLabel) {
      if (item.style.background.a > 0) DrawRectangleRec(item.bounds, item.style.background);
      if (item.style.borderThickness > 0.f && item.style.border.a > 0) DrawRectangleLinesEx(item.bounds, item.style.borderThickness, item.style.border);
    }

    if (item.text.empty()) continue;

    const Rectangle content = shrink(item.bounds, item.padding);
    const float fontSize = static_cast<float>(item.style.fontSize);
    if (item.kind == NodeKind::WrappedLabel) {
      std::istringstream stream(item.text);
      std::string word;
      std::string line;
      float y = content.y;
      const float lineHeight = fontSize + 4.f;
      while (stream >> word) {
        const std::string candidate = line.empty() ? word : line + " " + word;
        if (MeasureTextEx(font, candidate.c_str(), fontSize, textSpacing()).x > content.width && !line.empty()) {
          DrawTextEx(font, line.c_str(), {content.x, y}, fontSize, textSpacing(), item.style.text);
          y += lineHeight;
          line = word;
        } else {
          line = candidate;
        }
      }
      if (!line.empty()) DrawTextEx(font, line.c_str(), {content.x, y}, fontSize, textSpacing(), item.style.text);
      continue;
    }

    const Vector2 size = MeasureTextEx(font, item.text.c_str(), fontSize, textSpacing());
    float textX = content.x;
    if (item.kind == NodeKind::Button || item.kind == NodeKind::Field) textX += std::max(0.f, (content.width - size.x) * 0.5f);
    const Vector2 pos{textX, content.y + std::max(0.f, (content.height - size.y) * 0.5f)};
    DrawTextEx(font, item.text.c_str(), pos, fontSize, textSpacing(), item.style.text);
  }
}

const Item* Context::findItem(Id id) const
{
  for (const Item& item : m_items) {
    if (item.id == id) return &item;
  }
  return nullptr;
}

bool Context::clicked(Id id) const
{
  return m_interaction.clicked == id;
}

Rectangle Context::contentRect(const Node& node) const
{
  return shrink(node.bounds, node.layout.padding);
}

Vector2 Context::measureLeaf(const Node& node, float availableWidth) const
{
  const Font font = resolveFont(m_font);
  const float fontSize = static_cast<float>(node.style.fontSize);
  const Rectangle content = shrink(Rectangle{0.f, 0.f, availableWidth, 100000.f}, node.layout.padding);
  if (node.kind == NodeKind::WrappedLabel) {
    const float textHeight = measureWrappedTextHeight(node.text, std::max(1.f, content.width), node.style.fontSize);
    return {
      std::max(1.f, std::min(availableWidth, content.width + node.layout.padding.left + node.layout.padding.right)),
      textHeight + node.layout.padding.top + node.layout.padding.bottom,
    };
  }
  const Vector2 textSize = MeasureTextEx(font, node.text.c_str(), fontSize, textSpacing());
  return {
    textSize.x + node.layout.padding.left + node.layout.padding.right,
    textSize.y + node.layout.padding.top + node.layout.padding.bottom,
  };
}

float Context::measureWrappedTextHeight(const std::string& text, float maxWidth, int fontSize) const
{
  const Font font = resolveFont(m_font);
  std::istringstream stream(text);
  std::string word;
  std::string line;
  int lineCount = 0;
  while (stream >> word) {
    const std::string candidate = line.empty() ? word : line + " " + word;
    if (MeasureTextEx(font, candidate.c_str(), static_cast<float>(fontSize), textSpacing()).x > maxWidth && !line.empty()) {
      ++lineCount;
      line = word;
    } else {
      line = candidate;
    }
  }
  if (!line.empty()) ++lineCount;
  if (lineCount == 0) lineCount = 1;
  return lineCount * (static_cast<float>(fontSize) + 4.f);
}

float Context::resolveSize(const Size& size, float available, float fitSize) const
{
  switch (size.mode) {
    case SizeMode::Pixels: return size.value;
    case SizeMode::Grow: return available;
    case SizeMode::Fit: return fitSize;
    case SizeMode::Percent: return available * size.value;
  }
  return fitSize;
}

void Context::attachChild(int parentIndex, int childIndex)
{
  Node& parent = m_nodes[parentIndex];
  Node& child = m_nodes[childIndex];
  child.nextSibling = -1;
  if (parent.firstChild < 0) {
    parent.firstChild = childIndex;
    parent.lastChild = childIndex;
    return;
  }
  m_nodes[parent.lastChild].nextSibling = childIndex;
  parent.lastChild = childIndex;
}

void Context::detachChild(int parentIndex, int childIndex)
{
  Node& parent = m_nodes[parentIndex];
  int previous = -1;
  for (int child = parent.firstChild; child >= 0; child = m_nodes[child].nextSibling) {
    if (child != childIndex) {
      previous = child;
      continue;
    }
    const int next = m_nodes[child].nextSibling;
    if (previous >= 0) m_nodes[previous].nextSibling = next;
    else parent.firstChild = next;
    if (parent.lastChild == childIndex) parent.lastChild = previous;
    m_nodes[child].nextSibling = -1;
    return;
  }
}

int Context::ensureRoot()
{
  if (!m_nodes.empty()) return 0;
  Node root;
  root.id = kNoId;
  root.kind = NodeKind::Box;
  root.layout.width = Size::grow();
  root.layout.height = Size::grow();
  m_nodes.push_back(root);
  return 0;
}

int Context::findNodeIndex(Id id) const
{
  if (id == kNoId) return -1;
  const auto it = m_nodeLookup.find(id);
  if (it == m_nodeLookup.end()) return -1;
  return it->second;
}

void Context::markLayoutDirty(int nodeIndex)
{
  if (nodeIndex < 0 || nodeIndex >= static_cast<int>(m_nodes.size())) return;
  m_nodes[nodeIndex].layoutDirty = true;
  m_nodes[nodeIndex].subtreeDirty = true;

  int ancestor = nodeIndex;
  while (ancestor >= 0) {
    m_nodes[ancestor].subtreeDirty = true;
    const int parent = m_nodes[ancestor].parent;
    if (parent < 0) break;

    const bool parentSizeDependsOnChildren =
      m_nodes[parent].layout.width.mode == SizeMode::Fit ||
      m_nodes[parent].layout.height.mode == SizeMode::Fit;
    if (!parentSizeDependsOnChildren) break;

    m_nodes[parent].layoutDirty = true;
    ancestor = parent;
  }
}

void Context::clearDirtySubtree(int nodeIndex)
{
  Node& node = m_nodes[nodeIndex];
  node.layoutDirty = false;
  node.subtreeDirty = false;
  for (int child = node.firstChild; child >= 0; child = m_nodes[child].nextSibling) clearDirtySubtree(child);
}

bool Context::textAffectsLayout(const Node& node) const
{
  return node.kind == NodeKind::WrappedLabel ||
         node.layout.width.mode == SizeMode::Fit ||
         node.layout.height.mode == SizeMode::Fit;
}

bool Context::styleAffectsLayout(const Node& node, const Style& previous, const Style& next) const
{
  return previous.fontSize != next.fontSize && textAffectsLayout(node);
}

void Context::layoutNode(int nodeIndex, Rectangle bounds, bool force)
{
  Node& node = m_nodes[nodeIndex];
  if (!force && !node.layoutDirty && !node.subtreeDirty) return;

  const bool relayoutSelf = force || node.layoutDirty;
  if (relayoutSelf) node.bounds = bounds;

  if (node.firstChild < 0) {
    node.layoutDirty = false;
    node.subtreeDirty = false;
    return;
  }

  if (!relayoutSelf) {
    for (int child = node.firstChild; child >= 0; child = m_nodes[child].nextSibling) {
      if (m_nodes[child].layoutDirty || m_nodes[child].subtreeDirty) layoutNode(child, m_nodes[child].bounds, false);
    }
    node.subtreeDirty = false;
    for (int child = node.firstChild; child >= 0; child = m_nodes[child].nextSibling) {
      if (m_nodes[child].layoutDirty || m_nodes[child].subtreeDirty) {
        node.subtreeDirty = true;
        break;
      }
    }
    return;
  }

  const Rectangle inner = contentRect(node);
  const Axis axis = node.layout.axis;
  const float innerMain = mainAxisSize(inner, axis);
  const float innerCross = crossAxisSize(inner, axis);

  int childCount = 0;
  for (int child = node.firstChild; child >= 0; child = m_nodes[child].nextSibling) ++childCount;
  const float totalGap = node.layout.gap * std::max(0, childCount - 1);
  float remainingMain = std::max(0.f, innerMain - totalGap);
  float growWeight = 0.f;

  for (int child = node.firstChild; child >= 0; child = m_nodes[child].nextSibling) {
    const Node& childNode = m_nodes[child];
    const Size& mainSizeDef = axis == Axis::Horizontal ? childNode.layout.width : childNode.layout.height;
    if (mainSizeDef.mode == SizeMode::Grow) {
      growWeight += std::max(0.001f, mainSizeDef.value);
      continue;
    }

    const Vector2 fit = childNode.firstChild >= 0 ? Vector2{0.f, 0.f} : measureLeaf(childNode, inner.width);
    const float fitMain = axis == Axis::Horizontal ? fit.x : fit.y;
    remainingMain -= std::max(0.f, resolveSize(mainSizeDef, innerMain, fitMain));
  }
  remainingMain = std::max(0.f, remainingMain);

  float cursor = axis == Axis::Horizontal ? inner.x : inner.y;
  for (int child = node.firstChild; child >= 0; child = m_nodes[child].nextSibling) {
    Node& childNode = m_nodes[child];
    const Vector2 fit = childNode.firstChild >= 0 ? Vector2{0.f, 0.f} : measureLeaf(childNode, inner.width);

    const Size& mainSizeDef = axis == Axis::Horizontal ? childNode.layout.width : childNode.layout.height;
    const Size& crossSizeDef = axis == Axis::Horizontal ? childNode.layout.height : childNode.layout.width;

    float childMain = 0.f;
    if (mainSizeDef.mode == SizeMode::Grow) {
      const float weight = std::max(0.001f, mainSizeDef.value);
      childMain = growWeight > 0.f ? remainingMain * (weight / growWeight) : 0.f;
    } else {
      childMain = resolveSize(mainSizeDef, innerMain, axis == Axis::Horizontal ? fit.x : fit.y);
    }

    const float fitCross = axis == Axis::Horizontal ? fit.y : fit.x;
    float childCross = 0.f;
    if (crossSizeDef.mode == SizeMode::Grow || crossSizeDef.mode == SizeMode::Percent) childCross = resolveSize(crossSizeDef, innerCross, fitCross);
    else if (crossSizeDef.mode == SizeMode::Fit) childCross = fitCross;
    else childCross = crossSizeDef.value;

    if (childNode.layout.crossAlign == Align::Stretch || crossSizeDef.mode == SizeMode::Grow) childCross = innerCross;
    childCross = std::clamp(childCross, 0.f, innerCross);

    Rectangle childBounds{};
    if (axis == Axis::Horizontal) {
      childBounds = {cursor, inner.y, childMain, childCross};
      if (childNode.layout.crossAlign == Align::Center) childBounds.y = inner.y + (innerCross - childCross) * 0.5f;
      if (childNode.layout.crossAlign == Align::End) childBounds.y = inner.y + (innerCross - childCross);
    } else {
      childBounds = {inner.x, cursor, childCross, childMain};
      if (childNode.layout.crossAlign == Align::Center) childBounds.x = inner.x + (innerCross - childCross) * 0.5f;
      if (childNode.layout.crossAlign == Align::End) childBounds.x = inner.x + (innerCross - childCross);
    }

    layoutNode(child, childBounds, true);
    cursor += childMain + node.layout.gap;
  }

  node.layoutDirty = false;
  node.subtreeDirty = false;
}

void Context::collectItems()
{
  m_items.clear();
  m_items.reserve(m_nodes.size() > 0 ? m_nodes.size() - 1 : 0);
  for (size_t i = 1; i < m_nodes.size(); ++i) {
    const Node& node = m_nodes[i];
    m_items.push_back(Item{node.id, node.kind, node.bounds, node.layout.padding, node.style, node.text});
  }
}

void Context::updateInteraction()
{
  m_interaction = {};
  for (auto it = m_items.rbegin(); it != m_items.rend(); ++it) {
    if (CheckCollisionPointRec(m_mouse, it->bounds)) {
      m_interaction.hovered = it->id;
      m_interaction.wantsMouseCapture = true;
      break;
    }
  }
  if (m_mouseDown) m_interaction.pressed = m_interaction.hovered;
  if (m_mousePressed) m_interaction.clicked = m_interaction.hovered;
  if (m_mouseReleased && m_interaction.hovered == kNoId) m_interaction.pressed = kNoId;
}

} // namespace ui

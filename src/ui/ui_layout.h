#ifndef ASTEROIDS_UI_LAYOUT_H
#define ASTEROIDS_UI_LAYOUT_H

#include <raylib.h>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ui {

using Id = uint32_t;
constexpr Id kNoId = 0;

enum class Axis {
  Horizontal,
  Vertical,
};

enum class SizeMode {
  Pixels,
  Grow,
  Fit,
  Percent,
};

enum class Align {
  Start,
  Center,
  End,
  Stretch,
};

enum class NodeKind {
  Box,
  Label,
  WrappedLabel,
  Button,
  Field,
};

struct Size {
  SizeMode mode = SizeMode::Fit;
  float value = 0.f;

  static Size pixels(float value);
  static Size grow(float weight = 1.f);
  static Size fit();
  static Size percent(float value);
};

struct Edges {
  float left = 0.f;
  float top = 0.f;
  float right = 0.f;
  float bottom = 0.f;

  static Edges all(float value);
  static Edges symmetric(float horizontal, float vertical);
};

struct Layout {
  Axis axis = Axis::Vertical;
  Size width = Size::fit();
  Size height = Size::fit();
  Align crossAlign = Align::Stretch;
  Edges padding{};
  float gap = 0.f;
};

struct Style {
  Color background{0, 0, 0, 0};
  Color border{0, 0, 0, 0};
  Color text{255, 255, 255, 255};
  float borderThickness = 0.f;
  int fontSize = 18;
};

struct Item {
  Id id = kNoId;
  NodeKind kind = NodeKind::Box;
  Rectangle bounds{};
  Edges padding{};
  Style style{};
  std::string text;
};

struct Interaction {
  Id hovered = kNoId;
  Id pressed = kNoId;
  Id clicked = kNoId;
  bool wantsMouseCapture = false;
};

class Context {
 public:
  void clear();
  void setFont(const Font* font) { m_font = font; }
  void beginFrame(Rectangle rootBounds, Vector2 mouse, bool mousePressed, bool mouseDown, bool mouseReleased);
  void setRootLayout(const Layout& layout);
  void upsertNode(Id id, Id parentId, NodeKind kind, const Layout& layout, const Style& style = {}, const char* text = nullptr);
  void setLayout(Id id, const Layout& layout);
  void setStyle(Id id, const Style& style);
  void setText(Id id, const char* text);
  void compute();
  void draw() const;

  const std::vector<Item>& items() const { return m_items; }
  const Interaction& interaction() const { return m_interaction; }
  const Item* findItem(Id id) const;
  bool clicked(Id id) const;

 private:
  struct Node {
    Id id = kNoId;
    NodeKind kind = NodeKind::Box;
    Layout layout{};
    Style style{};
    std::string text;
    int parent = -1;
    int firstChild = -1;
    int lastChild = -1;
    int nextSibling = -1;
    Rectangle bounds{};
    bool layoutDirty = true;
    bool subtreeDirty = true;
  };

  Rectangle contentRect(const Node& node) const;
  Vector2 measureLeaf(const Node& node, float availableWidth) const;
  float measureWrappedTextHeight(const std::string& text, float maxWidth, int fontSize) const;
  float resolveSize(const Size& size, float available, float fitSize) const;
  void attachChild(int parentIndex, int childIndex);
  void detachChild(int parentIndex, int childIndex);
  int ensureRoot();
  int findNodeIndex(Id id) const;
  void markLayoutDirty(int nodeIndex);
  void clearDirtySubtree(int nodeIndex);
  bool textAffectsLayout(const Node& node) const;
  bool styleAffectsLayout(const Node& node, const Style& previous, const Style& next) const;
  void layoutNode(int nodeIndex, Rectangle bounds, bool force);
  void collectItems();
  void updateInteraction();

  std::vector<Node> m_nodes;
  std::unordered_map<Id, int> m_nodeLookup;
  std::vector<Item> m_items;
  const Font* m_font = nullptr;
  Rectangle m_rootBounds{};
  Vector2 m_mouse{};
  bool m_mousePressed = false;
  bool m_mouseDown = false;
  bool m_mouseReleased = false;
  Interaction m_interaction{};
};

} // namespace ui

#endif

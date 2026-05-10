#ifndef RAYLIB_OPERATORS_HPP
#define RAYLIB_OPERATORS_HPP

#include "assert.h"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>

// negation
inline Vector2 &operator-(Vector2 &a) {
  a = Vector2Negate(a);
  return a;
}

inline Vector3 &operator-(Vector3 &a) {
  a = Vector3Negate(a);
  return a;
}

// more math
inline float dot(const Vector2& a, const Vector2& b)
{
  return a.x*b.x + a.y+b.y;
}
inline Vector2 proj(const Vector2& a, const Vector2& b)
{
  return dot(a,b)/dot(b,b) * b;
}

inline double CyclicDistSquared(const Vector2& p0, const Vector2& p1,
                         const Vector2& bounds)
{
  float dx = std::min(std::abs(p0.x-p1.x),bounds.x-std::abs(p0.x-p1.x));
  float dy = std::min(std::abs(p0.y-p1.y),bounds.y-std::abs(p0.y-p1.y));
  return (dx*dx + dy*dy);
}
inline double CyclicDist(const Vector2& p0, const Vector2& p1,
                         const Vector2& bounds)
{
  float dx = std::min(std::abs(p0.x-p1.x),bounds.x-std::abs(p0.x-p1.x));
  float dy = std::min(std::abs(p0.y-p1.y),bounds.y-std::abs(p0.y-p1.y));
  return sqrt(dx*dx + dy*dy);
}
inline Vector2 CyclicDirTo(const Vector2& from, const Vector2& to,
                    const Vector2& bounds)
{
  Vector2 dir = to - from;
  dir.x = std::abs(dir.x) < bounds.x ? dir.x : -dir.x;
  dir.y = std::abs(dir.y) < bounds.y ? dir.y : -dir.y;
  return dir;
}

inline float Vector2Cross(const Vector2& a, const Vector2& b)
{
  return a.y*b.x-a.x*b.y;
}

#endif

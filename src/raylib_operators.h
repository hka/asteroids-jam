#ifndef RAYLIB_OPERATORS_HPP
#define RAYLIB_OPERATORS_HPP

#include "assert.h"
#include "raylib.h"
#include "raymath.h"

// addition
inline Vector2 operator+(const Vector2 &a, const Vector2 &b) {
  return Vector2Add(a, b);
}

inline Vector3 operator+(const Vector3 &a, const Vector3 &b) {
  return Vector3Add(a, b);
}

inline Vector4 operator+(const Vector4 &a, const Vector4 &b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

inline Vector2 &operator+=(Vector2 &a, const Vector2 &b) {
  a = a + b;
  return a;
}

inline Vector3 &operator+=(Vector3 &a, const Vector3 &b) {
  a = a + b;
  return a;
}

inline Vector4 &operator+=(Vector4 &a, const Vector4 &b) {
  a = a + b;
  return a;
}

inline Matrix operator+(const Matrix &a, const Matrix &b) {
  return MatrixAdd(a, b);
}

inline Matrix &operator+=(Matrix &a, const Matrix &b) {
  a = MatrixAdd(a, b);
  return a;
}

// subtraction
inline Vector2 operator-(const Vector2 &a, const Vector2 &b) {
  return Vector2Subtract(a, b);
}

inline Vector3 operator-(const Vector3 &a, const Vector3 &b) {
  return Vector3Subtract(a, b);
}

inline Vector4 operator-(const Vector4 &a, const Vector4 &b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

inline Vector2 &operator-=(Vector2 &a, const Vector2 &b) {
  a = a - b;
  return a;
}

inline Vector3 &operator-=(Vector3 &a, const Vector3 &b) {
  a = a - b;
  return a;
}

inline Vector4 &operator-=(Vector4 &a, const Vector4 &b) {
  a = a - b;
  return a;
}

inline Matrix operator-(const Matrix &left, const Matrix &right) {
  return MatrixSubtract(left, right);
}

inline Matrix &operator-=(Matrix &left, const Matrix &right) {
  left = MatrixSubtract(left, right);
  return left;
}

// negation
inline Vector2 &operator-(Vector2 &a) {
  a = Vector2Negate(a);
  return a;
}

inline Vector3 &operator-(Vector3 &a) {
  a = Vector3Negate(a);
  return a;
}

// scaling
inline Vector2 operator*(const Vector2 &v, float s) {
  return Vector2Scale(v, s);
}

inline Vector2 operator*(const float &s, Vector2 v) {
  return Vector2Scale(v, s);
}

inline Vector3 operator*(const Vector3 &a, float b) {
  return Vector3Scale(a, b);
}

inline Vector3 operator*(const float &s, Vector3 v) {
  return Vector3Scale(v, s);
}

inline Vector2 &operator*=(Vector2 &a, const float b) {
  a = a * b;
  return a;
}

inline Vector3 &operator*=(Vector3 &a, const float b) {
  a = a * b;
  return a;
}

// multiplication
inline Matrix operator*(const Matrix &left, const Matrix &right) {
  return MatrixMultiply(left, right);
}

inline Matrix &operator*=(Matrix &left, const Matrix &right) {
  left = MatrixMultiply(left, right);
  return left;
}

// division
inline Vector2 operator/(const Vector2 &a, const float b) {
  assert(b != 0.0);
  float recip = 1.0 / b;
  return a * recip;
}

inline Vector3 operator/(const Vector3 &a, const float b) {
  assert(b != 0.0);
  float recip = 1.0 / b;
  return a * recip;
}

inline Vector2 &operator/=(Vector2 &a, const float b) {
  a = a / b;
  return a;
}

inline Vector3 &operator/=(Vector3 &a, const float b) {
  a = a / b;
  return a;
}

#endif

#include "Player.hpp"
#include "globals.h"
#include "helpers.h"
#include "raylib_operators.h"

PlayerSteer createPlayer(Vector2 startPos){
  PlayerSteer player;
  player.position = startPos;

  player.movement.maxAcceleration = 250.f;
  player.movement.direction = {0.f, -1.f};
  player.movement.force = {0.f, 0.f};
  player.movement.velocity = {0.f, 0.f};
  player.movement.currentAcceleration = 0.f;
  player.movement.accelerationDecrease = 10.f;
  player.movement.rotation = PI / 2.f;
  player.movement.rotationSpeed = 5.f;

  player.suckAttack.isOngoing = false;
  player.suckAttack.lineLength = 50.f;
  return player;
}

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void update(PlayerSteer& player, const Vector2& worldBound){
  RotateShip(player.movement.direction, player.movement.rotationSpeed);
  accelerate(player.movement);
  UpdateMovement(player.position, player.movement, worldBound);

  suckAttack(player.position, player.movement.rotation, player.suckAttack);

  Vector2 mousePointer = GetMousePosition();
  player.gunDirection = Vector2Normalize(mousePointer-player.position);
  if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
  {
    FireShoot(player);
  }
}

////////////////////////////////////////////////
///         Input                           ///
///////////////////////////////////////////////

void RotateShip(Vector2 &direction, float rotationSpeed){
  float angle = 0.f;

  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)){
    angle = -rotationSpeed * GetFrameTime();
  }else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)){
    angle = rotationSpeed * GetFrameTime();
  }

  if(angle == 0.f){
    return;
  }

  float cosAngle = cos(angle);
  float sinAngle = sin(angle);
  float newX = direction.x * cosAngle - direction.y * sinAngle;
  float newY = direction.x * sinAngle + direction.y * cosAngle;

  direction = Vector2Normalize({newX, newY});
}

void accelerate(MovementComponent &movement)
{

  float dampingFactor = 0.01f;
  if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)){
    if (Vector2LengthSqr(movement.velocity) > 0.001f){
      movement.force = Vector2Scale(Vector2Normalize(movement.velocity), -(dampingFactor*2.f) * Vector2Length(movement.velocity));
    }else{
      movement.force = Vector2Zero();
    }
  }else if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)){
    movement.force = Vector2Scale(movement.direction, 2.f);
  }else{
    if(Vector2LengthSqr(movement.velocity) > 0.001f){
      movement.force = Vector2Scale(Vector2Normalize(movement.velocity), -dampingFactor * Vector2Length(movement.velocity));
    }else{
      movement.force = Vector2Zero();
    }
  }
}

void suckAttack(const Vector2 &position, const float rotation, SuckAttack &suckAttack)
{
  if (!IsKeyDown(KEY_SPACE))
  {
    suckAttack.isOngoing = false;
    suckAttack.balls.clear();
    return;
  }

  if (!suckAttack.isOngoing)
  {
    suckAttack.addBallTimer.start();
  }

  Vector2 startPoint = position + 18 * Vector2Normalize({cosf(rotation), sinf(rotation)});

  suckAttack.isOngoing = true;
  float angle = PI / 4.f;
  Vector2 dir1 = Vector2Normalize({cosf(rotation - angle), sinf(rotation - angle)});
  Vector2 dir2 = Vector2Normalize({cosf(rotation + angle), sinf(rotation + angle)});
  float lineLength = suckAttack.lineLength;
  suckAttack.linesEnd[0] = {startPoint.x + (dir1.x * lineLength), startPoint.y + (dir1.y * lineLength)};
  suckAttack.linesEnd[1] = {startPoint.x + (dir2.x * lineLength), startPoint.y + (dir2.y * lineLength)};

  if (suckAttack.addBallTimer.getElapsed() >= 0.05f)
  {
    Vector2 startPos = RandomPositionBetweenPoints(suckAttack.linesEnd[0], suckAttack.linesEnd[1]);
    float initRadius = 1.f;
    float initSpeed = 250.f;
    Ball ball = {startPos, initRadius, initSpeed};
    suckAttack.balls.emplace_back(ball);

    suckAttack.addBallTimer.start();
  }

  for (std::size_t i = 0; i < suckAttack.balls.size(); ++i)
  {
    moveBallTowardsPoint(suckAttack.balls[i], startPoint);
    float dist = Vector2Distance(suckAttack.balls[i].position, startPoint);
    if (dist <= 2.f || dist > lineLength)
    {
      suckAttack.balls[i] = suckAttack.balls[suckAttack.balls.size() - 1];
      suckAttack.balls.pop_back();
    }
  }
}

////////////////////////////////////////////////
///         Helper                          ///
///////////////////////////////////////////////
void rotateTriangle(Vector2 (&v)[3], const float angle){
  Vector2 center = {(v[0].x + v[1].x + v[2].x) / 3.f, (v[0].y + v[1].y + v[2].y) / 3};
  v[0] = Vector2Add(Vector2Rotate(Vector2Subtract(v[0], center), angle), center);
  v[1] = Vector2Add(Vector2Rotate(Vector2Subtract(v[1], center), angle), center);
  v[2] = Vector2Add(Vector2Rotate(Vector2Subtract(v[2], center), angle), center);
}

void rotateTriangle(Vector2 (&v)[3], const float angle, const Vector2& center){
  v[0] = Vector2Add(Vector2Rotate(Vector2Subtract(v[0], center), angle), center);
  v[1] = Vector2Add(Vector2Rotate(Vector2Subtract(v[1], center), angle), center);
  v[2] = Vector2Add(Vector2Rotate(Vector2Subtract(v[2], center), angle), center);
}

void rotateTriangle(Vector2 (&v)[3], const Vector2 &direction, const Vector2 &center)
{
  Vector2 forward = {0.f, 0.f};
  float angle = atan2(direction.y, direction.x) - atan2(forward.y, forward.x);

  // Rotate each vertex
  for (int i = 0; i < 3; ++i)
  {
    Vector2 relativePos = Vector2Subtract(v[i], center);
    float cosAngle = cos(angle);
    float sinAngle = sin(angle);
    float newX = relativePos.x * cosAngle - relativePos.y * sinAngle;
    float newY = relativePos.x * sinAngle + relativePos.y * cosAngle;
    v[i] = Vector2Add({newX, newY}, center);
  }
}

Vector2 RandomPositionBetweenPoints(Vector2 point1, Vector2 point2)
{
  float randomFactor = GetRandomValue(0, 100) / 100.f; // Generate a random factor between 0 and 1

  Vector2 randomPosition;
  randomPosition.x = point1.x + (point2.x - point1.x) * randomFactor;
  randomPosition.y = point1.y + (point2.y - point1.y) * randomFactor;

  return randomPosition;
}

void moveBallTowardsPoint(Ball &ball, Vector2 targetPoint)
{
  Vector2 direction = Vector2Normalize({targetPoint.x - ball.position.x, targetPoint.y - ball.position.y});

  ball.position.x += direction.x * ball.speed * GetFrameTime();
  ball.position.y += direction.y * ball.speed * GetFrameTime();
}

////////////////////////////////////////////////
///         Paint                            ///
///////////////////////////////////////////////
void DrawShip(const PlayerSteer& player)
{
  Vector2 bound = {(float)options.screenWidth, (float)options.screenWidth};

  Vector2 vertices[3];
  vertices[0] = player.position + Vector2{15.f, 0.f};
  vertices[1] = {player.position.x - 15.f, player.position.y - 10.f};
  vertices[2] = {player.position.x - 15.f, player.position.y + 10.f};
  rotateTriangle(vertices, player.movement.direction, player.position);

  // draw 4 times as a hack to handle wrapping
  DrawTriangle(
    vertices[0],
    vertices[1],
    vertices[2],
    GREEN
    );
  Vector2 p;
  Vector2 off = bound;
  off.x = 0;
  p = mod(player.position + off,bound);
  vertices[0] = p + Vector2{15.f, 0.f};
  vertices[1] = {p.x - 15.f, p.y - 10.f};
  vertices[2] = {p.x - 15.f, p.y + 10.f};
  rotateTriangle(vertices, player.movement.direction, player.position);
  DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);

  off = bound;
  off.y = 0;
  p = mod(player.position + off,bound);
  vertices[0] = p + Vector2{15.f, 0.f};
  vertices[1] = {p.x - 15.f, p.y - 10.f};
  vertices[2] = {p.x - 15.f, p.y + 10.f};
  rotateTriangle(vertices, player.movement.direction, player.position);
  DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);

  off = bound;
  p = mod(player.position + off,bound);
  vertices[0] = p + Vector2{15.f, 0.f};
  vertices[1] = {p.x - 15.f, p.y - 10.f};
  vertices[2] = {p.x - 15.f, p.y + 10.f};
  rotateTriangle(vertices, player.movement.direction, player.position);
  DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);
}

void DrawGun(const PlayerSteer& player)
{
  Vector2 bound = {(float)options.screenWidth, (float)options.screenWidth};

  DrawCircleV(player.position, 5,RED);

  DrawLineEx(player.position, player.position + 10*player.gunDirection ,3, GRAY);
}

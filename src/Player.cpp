#include "Player.hpp"
#include "globals.h"
#include "helpers.h"
#include "raylib_operators.h"

PlayerSteer createPlayer(Vector2 startPos){
  PlayerSteer player;
  player.position = startPos;

  player.movement.maxAcceleration = 250.f;
  player.movement.velocity = {0.f, 0.f};
  player.movement.currentAcceleration = 0.f;
  player.movement.accelerationDecrease = 10.f;
  player.movement.rotation = 0.f;
  player.movement.rotationSpeed = 5.f;

  player.suckAttack.isOngoing = false;
  player.suckAttack.lineLength = 50.f;
  return player;
}

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void update(PlayerSteer& player, const Vector2& worldBound){
  rotateShip(player.movement);
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
void rotateShip(MovementComponent& movement){
  if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)){
    float angle = movement.rotationSpeed * GetFrameTime();
    movement.rotation -= angle;
  }else if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)){
    float angle = movement.rotationSpeed * GetFrameTime();
    movement.rotation += angle;
  }
}

void accelerate(MovementComponent &movement)
{
  if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)){
    float newAccleration = movement.currentAcceleration - (movement.accelerationDecrease * 2.f);
    movement.currentAcceleration = newAccleration > 0.f ? newAccleration : 0.f;
  }else if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)){
    movement.currentAcceleration = movement.maxAcceleration;
  }else{
    float newAccleration = movement.currentAcceleration - movement.accelerationDecrease;
    movement.currentAcceleration = newAccleration > 0.f ? newAccleration : 0.f;
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
  rotateTriangle(vertices, player.movement.rotation, player.position);

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
  rotateTriangle(vertices, player.movement.rotation,p);
  DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);

  off = bound;
  off.y = 0;
  p = mod(player.position + off,bound);
  vertices[0] = p + Vector2{15.f, 0.f};
  vertices[1] = {p.x - 15.f, p.y - 10.f};
  vertices[2] = {p.x - 15.f, p.y + 10.f};
  rotateTriangle(vertices, player.movement.rotation, p);
  DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);

  off = bound;
  p = mod(player.position + off,bound);
  vertices[0] = p + Vector2{15.f, 0.f};
  vertices[1] = {p.x - 15.f, p.y - 10.f};
  vertices[2] = {p.x - 15.f, p.y + 10.f};
  rotateTriangle(vertices, player.movement.rotation, p);
  DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);
}

void DrawGun(const PlayerSteer& player)
{
  Vector2 bound = {(float)options.screenWidth, (float)options.screenWidth};

  DrawCircleV(player.position, 5,RED);

  DrawLineEx(player.position, player.position + 10*player.gunDirection ,3, GRAY);
}

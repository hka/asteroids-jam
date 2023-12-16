#include "Player.hpp"
#include "globals.h"
#include "helpers.h"
#include "raylib_operators.h"

PlayerState createPlayer(Vector2 startPos){
  PlayerState player;
  player.data.position = startPos;
  float radius = 15.f;
  player.data.radius = radius;
  player.data.mass = M_PI*radius*radius;
  player.data.drag = 5;
  player.data.orientation = {0.f, -1.f};

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

  player.gun.direction = {0.f,0.f};
  player.gun.cooldownTimer.start();
  player.gun.cooldownDuration = 0.25f;

  return player;
}

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void update(PlayerState &player, const Vector2 &worldBound, std::vector<Shoot> &shoots, float dt)
{
  UpdatePlayerInput(player.data, dt);
  player.data.force *= 0;
  ApplyThrustDrag(player.data);
  UpdatePosition(player.data, worldBound, dt);


  suckAttack(player.data.position, player.data.orientation, player.suckAttack);
  gunUpdate(player, player.gun, shoots);
}

////////////////////////////////////////////////
///         Input                           ///
///////////////////////////////////////////////
void UpdatePlayerInput(PhysicsComponent& data, float dt)
{
  //rotate velocity vector and update orientation to match
  //TODO handle rotation when moving in reverse?
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
  {
    data.velocity = Vector2Rotate(data.velocity, -(M_PI/0.8f)*dt);
    if(Vector2LengthSqr(data.velocity)>0)
    {
      data.orientation = Vector2Normalize(data.velocity);
    }
  }
  else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
  {
    data.velocity = Vector2Rotate(data.velocity, (M_PI/0.8f)*dt);
    if(Vector2LengthSqr(data.velocity)>0)
    {
      data.orientation = Vector2Normalize(data.velocity);
    }
  }

  if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
  {
    data.thrust = -500000;
  }
  else if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
  {
    data.thrust = 500000;
  }
  else
  {
    data.thrust = 0;
  }
}
#if 0
void RotateShip(Vector2 &direction, float rotationSpeed, float dt){
  float angle = 0.f;

  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)){
    angle = -rotationSpeed * dt;
  }else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)){
    angle = rotationSpeed * dt;
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
#endif

void suckAttack(const Vector2 &position, const Vector2& rotation, SuckAttack &suckAttack)
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

  Vector2 startPoint = position + 18 * rotation;

  suckAttack.isOngoing = true;
  float angle = PI / 4.f;
  Vector2 dir1 = Vector2Rotate(rotation,-angle);
  Vector2 dir2 = Vector2Rotate(rotation,angle);
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

void gunUpdate(const PlayerState& player, GunAttack &gun, std::vector<Shoot> &shoots)
{
  //update rotation
  Vector2 mousePointer = GetMousePosition();
  gun.direction = Vector2Normalize(mousePointer - player.data.position);

  //handle shooting
  if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && gun.cooldownTimer.getElapsed() >= gun.cooldownDuration){
    //FireShoot(player.data.position, gun.direction, player.movement.velocity, player.movement.maxAcceleration, shoots);
    FireShoot(player.data, gun.direction,500, shoots);
    gun.cooldownTimer.start();
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
void DrawShip(const PlayerState& player)
{
  Vector2 bound = {(float)options.screenWidth, (float)options.screenWidth};
  Vector2 pos = player.data.position;
  Vector2 vertices[3];
  vertices[0] = pos + Vector2{15.f, 0.f};
  vertices[1] = {pos.x - 15.f, pos.y - 10.f};
  vertices[2] = {pos.x - 15.f, pos.y + 10.f};
  rotateTriangle(vertices, player.data.orientation, pos);

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
  p = mod(pos + off,bound);
  vertices[0] = p + Vector2{15.f, 0.f};
  vertices[1] = {p.x - 15.f, p.y - 10.f};
  vertices[2] = {p.x - 15.f, p.y + 10.f};
  rotateTriangle(vertices, player.data.orientation, pos);
  DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);

  off = bound;
  off.y = 0;
  p = mod(pos + off,bound);
  vertices[0] = p + Vector2{15.f, 0.f};
  vertices[1] = {p.x - 15.f, p.y - 10.f};
  vertices[2] = {p.x - 15.f, p.y + 10.f};
  rotateTriangle(vertices, player.data.orientation, pos);
  DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);

  off = bound;
  p = mod(pos + off,bound);
  vertices[0] = p + Vector2{15.f, 0.f};
  vertices[1] = {p.x - 15.f, p.y - 10.f};
  vertices[2] = {p.x - 15.f, p.y + 10.f};
  rotateTriangle(vertices, player.data.orientation, pos);
  DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);
}

void DrawGun(const PlayerState& player)
{
  Vector2 bound = {(float)options.screenWidth, (float)options.screenWidth};

  DrawCircleV(player.data.position, 5,RED);

  DrawLineEx(player.data.position, player.data.position + 10*player.gun.direction ,3, GRAY);
}

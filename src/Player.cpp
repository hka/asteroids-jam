#include "Player.hpp"
#include "globals.h"
#include "helpers.h"
#include "raylib_operators.h"

PlayerSteer createPlayer(Vector2 startPos){
  PlayerSteer player;
  player.position = startPos;

  player.maxAcceleration = 250.f;
  player.velocity = {0.f,0.f};
  player.currentAcceleration = 0.f;
  player.accelerationDecrease = 10.f;
  player.rotation = 0.f;
  player.rotationSpeed = 5.f;

  player.suckAttack.isOngoing = false;
  player.suckAttack.lineLength = 50.f;
  return player;
}

void update(PlayerSteer& player){
  rotateShip(player);
  accelerate(player);
  updateVelocity(player.rotation, player.velocity, player.currentAcceleration);
  updatePosition(player.position, player.velocity, GetFrameTime());
  suckAttack(player.position, player.rotation, player.suckAttack);
}

void rotateShip(PlayerSteer& player){
  if(IsKeyDown(KEY_LEFT)){
    float angle = player.rotationSpeed * GetFrameTime();
    player.rotation -= angle;
  }else if(IsKeyDown(KEY_RIGHT)){
    float angle = player.rotationSpeed * GetFrameTime();
    player.rotation += angle;
  }
}

void accelerate(PlayerSteer& player){
  if(IsKeyDown(KEY_DOWN)){
    float newAccleration = player.currentAcceleration - (player.accelerationDecrease * 2.f);
    player.currentAcceleration = newAccleration > 0.f ? newAccleration : 0.f;
  }else if(IsKeyDown(KEY_UP)){
    player.currentAcceleration = player.maxAcceleration;
  }else{
    float newAccleration = player.currentAcceleration - player.accelerationDecrease;
    player.currentAcceleration = newAccleration > 0.f ? newAccleration : 0.f;
  }
}

void rotateTriangle(Vector2 (&v)[3], const float angle){
  Vector2 center = {(v[0].x + v[1].x + v[2].x) / 3.f, (v[0].y + v[1].y + v[2].y) / 3};
  v[0] = Vector2Add(Vector2Rotate(Vector2Subtract(v[0], center), angle), center);
  v[1] = Vector2Add(Vector2Rotate(Vector2Subtract(v[1], center), angle), center);
  v[2] = Vector2Add(Vector2Rotate(Vector2Subtract(v[2], center), angle), center);
}

void updateVelocity(const float rotation, Vector2& velocity, const float acceleration){
  Vector2 newVelocity = Vector2Normalize({cosf(rotation), sinf(rotation)});
  newVelocity.x *= acceleration;
  newVelocity.y *= acceleration;
  velocity = newVelocity;
}

void updatePosition(Vector2 &v, Vector2& velocity, const float dt)
{
  Vector2 newPos = Vector2Add(v,  Vector2Scale(velocity,dt));
  Vector2 bound = {(float)options.screenWidth, (float)options.screenHeight};
  v = mod(newPos+bound, bound);
}

void DrawShip(const PlayerSteer& player)
{
  Vector2 bound = {(float)options.screenWidth, (float)options.screenWidth};

  Vector2 vertices[3];
  vertices[0] = player.position + Vector2{15.f, 0.f};
  vertices[1] = {player.position.x - 15.f, player.position.y - 10.f};
  vertices[2] = {player.position.x - 15.f, player.position.y + 10.f};
  rotateTriangle(vertices, player.rotation);

  // draw 4 times as a hack to handle wrapping
  DrawTriangle(
    vertices[0],
    vertices[1],
    vertices[2],
    GREEN
    );
  Vector2 v[3];
  Vector2 off = bound;
  off.x = 0;
  v[0] = vertices[0] + off;
  v[1] = vertices[1] + off;
  v[2] = vertices[2] + off;
  DrawTriangle(v[0], v[1], v[2], GREEN);
  off = bound;
  off.y = 0;
  v[0] = vertices[0] + off;
  v[1] = vertices[1] + off;
  v[2] = vertices[2] + off;
  DrawTriangle(v[0], v[1], v[2], GREEN);
  off = bound;
  v[0] = vertices[0] + off;
  v[1] = vertices[1] + off;
  v[2] = vertices[2] + off;
  DrawTriangle(v[0], v[1], v[2], GREEN);
}

Vector2 RandomPositionBetweenPoints(Vector2 point1, Vector2 point2) {
  float randomFactor = GetRandomValue(0, 100) / 100.f; // Generate a random factor between 0 and 1

  Vector2 randomPosition;
  randomPosition.x = point1.x + (point2.x - point1.x) * randomFactor;
  randomPosition.y = point1.y + (point2.y - point1.y) * randomFactor;

  return randomPosition;
}

void moveBallTowardsPoint(Ball &ball, Vector2 targetPoint) {
  Vector2 direction = Vector2Normalize({targetPoint.x - ball.position.x, targetPoint.y - ball.position.y});

  ball.position.x += direction.x * ball.speed * GetFrameTime();
  ball.position.y += direction.y * ball.speed * GetFrameTime();
}

void suckAttack(const Vector2& position, const float rotation, SuckAttack& suckAttack){
  if(!IsKeyDown(KEY_SPACE)){
    suckAttack.isOngoing = false;
    suckAttack.balls.clear();
    return;
  }

  if(!suckAttack.isOngoing){
    suckAttack.addBallTimer.start();
  }

  Vector2 startPoint = position + 18*Vector2Normalize({cosf(rotation), sinf(rotation)});

  suckAttack.isOngoing = true;
  float angle = PI / 4.f;
  Vector2 dir1 = Vector2Normalize({cosf(rotation - angle), sinf(rotation - angle)});
  Vector2 dir2 = Vector2Normalize({cosf(rotation + angle), sinf(rotation + angle)});
  float lineLength = suckAttack.lineLength;
  suckAttack.linesEnd[0] = {startPoint.x + (dir1.x * lineLength), startPoint.y + (dir1.y * lineLength)};
  suckAttack.linesEnd[1] = {startPoint.x + (dir2.x * lineLength), startPoint.y + (dir2.y * lineLength)};


  if(suckAttack.addBallTimer.getElapsed() >= 0.05f){
    Vector2 startPos = RandomPositionBetweenPoints(suckAttack.linesEnd[0], suckAttack.linesEnd[1]);
    float initRadius = 1.f;
    float initSpeed = 250.f;
    Ball ball = {startPos, initRadius, initSpeed};
    suckAttack.balls.emplace_back(ball);

    suckAttack.addBallTimer.start();
  }

  for(std::size_t i = 0; i < suckAttack.balls.size(); ++i){
     moveBallTowardsPoint(suckAttack.balls[i], startPoint);
     float dist = Vector2Distance(suckAttack.balls[i].position, startPoint);
     if(dist <= 2.f || dist > lineLength){
      suckAttack.balls[i] = suckAttack.balls[suckAttack.balls.size() - 1];
      suckAttack.balls.pop_back();
     }
  }

}

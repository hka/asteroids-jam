#include "Player.hpp"
#include "globals.h"
#include "helpers.h"
#include "raylib_operators.h"

#define RAY2D_COLLISION_IMPLEMENTATION //only this once!
#include <ray_collision_2d.h>
#undef RAY2D_COLLISION_IMPLEMENTATION

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

  player.energy.maxValue = 100.f;
  player.energy.value = player.energy.maxValue;
  player.gun.energyCost = player.energy.maxValue * 0.025f;

  player.laser = createLaser();
  player.laserEnergy.maxValue = 100.f;
  player.laserEnergy.value = player.laserEnergy.maxValue;

  return player;
}

////////////////////////////////////////////////
///         Update                          ///
///////////////////////////////////////////////
void update(PlayerState &player, const Vector2 &worldBound, std::vector<Shoot> &shoots, float dt)
{
  UpdatePlayerInput(player, dt, player.energy);
  player.data.force *= 0;
  ApplyThrustDrag(player.data);
  UpdatePosition(player.data, worldBound, dt);

  if(IsMatchingKeyPressed(options.keys[(size_t)GameOptions::ControlKeyCodes::ABSORB]))
  {
    player.suckDelayTimer.start();
  }

  if(player.suckDelayTimer.getElapsed() > SUCK_DELAY){
    if(IsMatchingKeyDown(options.keys[(size_t)GameOptions::ControlKeyCodes::ABSORB]) && player.storedAsteroids < MAX_STORED_ASTEROIDS)
    {
      suckAttack(player.data.position, player.data.orientation, player.suckAttack);
    }else{
      player.suckAttack.isOngoing = false;
      player.suckAttack.balls.clear();
    }
  }
  else if(player.suckDelayTimer.getElapsed() < SUCK_DELAY  && IsMatchingKeyReleased(options.keys[(size_t)GameOptions::ControlKeyCodes::ABSORB]))
  {
    turnAsteroidToEnergy(player);
  }

  gunUpdate(player, player.gun, shoots);
  laserUpdate(player);
}

void UpdateEnergy(Energy& energy, float value){
  energy.value += value;

  energy.value = std::min(energy.value, energy.maxValue);
  energy.value = std::max(energy.value, 0.f);
}

////////////////////////////////////////////////
///         Input                           ///
///////////////////////////////////////////////
void UpdatePlayerInput(PlayerState& player, float dt, Energy& energy)
{
  PhysicsComponent& data = player.data;
  //rotate velocity vector and update orientation to match
  //TODO handle rotation when moving in reverse?
  if(IsMatchingKeyDown(options.keys[(size_t)GameOptions::ControlKeyCodes::TURN_LEFT]))
  {
    data.velocity = Vector2Rotate(data.velocity, -(M_PI/0.8f)*dt);
    if(Vector2LengthSqr(data.velocity)>0)
    {
      data.orientation = Vector2Normalize(data.velocity);
    }
  }
  else if(IsMatchingKeyDown(options.keys[(size_t)GameOptions::ControlKeyCodes::TURN_RIGHT]))
  {
    data.velocity = Vector2Rotate(data.velocity, (M_PI/0.8f)*dt);
    if(Vector2LengthSqr(data.velocity)>0)
    {
      data.orientation = Vector2Normalize(data.velocity);
    }
  }

  const float THRUST_ENERGY_COST = 0.1f; //todo move somewhere else
  if(IsMatchingKeyDown(options.keys[(size_t)GameOptions::ControlKeyCodes::BREAK]) && hasEnoughEnergy(energy, THRUST_ENERGY_COST))
  {
    UpdateEnergy(energy, -THRUST_ENERGY_COST);
    data.thrust = -500000;
  }
  else if(IsMatchingKeyDown(options.keys[(size_t)GameOptions::ControlKeyCodes::THRUST]) && hasEnoughEnergy(energy, THRUST_ENERGY_COST))
  {
    UpdateEnergy(energy, -THRUST_ENERGY_COST);
    data.thrust = 500000;
  }
  else
  {
    data.thrust = 0;
  }

  if(IsMatchingKeyDown(options.keys[(size_t)GameOptions::ControlKeyCodes::DASH]) && hasEnoughEnergy(energy, DASH_ENERGY_COST) && !player.dash_in_progress)
  {
    UpdateEnergy(energy, -DASH_ENERGY_COST);
    player.dash_in_progress = true;
  }
  else if(player.dash_in_progress && !IsMatchingKeyDown(options.keys[(size_t)GameOptions::ControlKeyCodes::DASH]))
  {
    player.dash_in_progress = false;
    data.position = data.position + data.orientation*options.screenWidth*DASH_DISTANCE;
  }
}

void AttractAsteroids(PlayerState& player, std::vector<Asteroid>& asteroids)
{
  //Vector2 attract_point = player.data.position + player.data.orientation*(player.data.radius+50);
  for(size_t ii = 0; ii < asteroids.size(); ++ii)
  {
    if(asteroids[ii].target == 1)
    {
      Vector2 attract_point = asteroids[ii].attract_point + player.data.orientation*50;
      Vector2 dir = attract_point - asteroids[ii].data.position;
      asteroids[ii].data.force += 500000*Vector2Normalize(dir);
      if(Vector2Length(dir) < 20)
      {
        asteroids[ii].data.velocity = player.data.velocity;
        asteroids[ii].data.force *= 0;

        asteroids[ii].state = ABSORBED;
        player.storedAsteroids = std::min(MAX_STORED_ASTEROIDS, player.storedAsteroids + 1);
      }
    }
    else if(asteroids[ii].target == 2)
    {
      Vector2 attract_point = asteroids[ii].attract_point + player.data.orientation*50;
      Vector2 dir = attract_point - asteroids[ii].data.position;
      asteroids[ii].data.force += 500000*Vector2Normalize(dir);
    }
  }
}

//here also targeting is done...
void PaintAttractAsteroids(PlayerState& player, std::vector<Asteroid>& asteroids, std::vector<float>& player_asteroid_distance)
{
  Vector2 attract_point = player.data.position + player.data.orientation*player.data.radius;

  //visualize cone
  float cone_angle = 15*M_PI/180; //should be part of player state and controllable
  float line_len = 400;
  Vector2 p1 = attract_point + Vector2Rotate(player.data.orientation,-cone_angle)*line_len;
  Vector2 p2 = attract_point + Vector2Rotate(player.data.orientation,cone_angle)*line_len;

  DrawLineEx(attract_point, p1, 1, RED);
  DrawLineEx(attract_point, p2, 1, RED);

  if(asteroids.size() != player_asteroid_distance.size())
  {
    return;
  }
  float attract_distance = 400;
  for(size_t ii = 0; ii < asteroids.size(); ++ii)
  {
    asteroids[ii].target = 0;
    if(asteroids[ii].type != 1)
    {
      continue;
    }
    if(player_asteroid_distance[ii] <= attract_distance)
    {
      //check if inside cone
      Vector2 A = p1 - attract_point;
      Vector2 B = asteroids[ii].data.position - attract_point;
      Vector2 C = p2 - attract_point;

      //or maybe wrapped cone
      Vector2 collision_point;
      float collision_dist;
      bool check_wrap = false;
      Rectangle bound = {0,0,(float)options.screenWidth, (float)options.screenHeight};
      Ray2d r;
      r.Origin = attract_point;
      r.Direction = player.data.orientation;
      r.Direction = -r.Direction;
      if(CheckCollisionRay2dRect(r, bound, &collision_point))
      {
        collision_dist = Vector2Length(r.Origin - collision_point);
        check_wrap = collision_dist < attract_distance;
      }

      if (Vector2Cross(A,B) * Vector2Cross(A,C) >= 0
          && Vector2Cross(C,B) * Vector2Cross(C,A) >= 0)
      {
        Rectangle asteroid_bound;
        float margin = 10;
        asteroid_bound.x = asteroids[ii].data.position.x - asteroids[ii].data.radius - margin;
        asteroid_bound.y = asteroids[ii].data.position.y - asteroids[ii].data.radius - margin;
        asteroid_bound.width = 2*(asteroids[ii].data.radius + margin);
        asteroid_bound.height = 2*(asteroids[ii].data.radius + margin);
        DrawRectangleLinesEx(asteroid_bound, 2, RED);
        asteroids[ii].target = 1;
        asteroids[ii].attract_point = attract_point;
      }
      else if(check_wrap) // for wrapping, check wrapped cone
      {
        Vector2 collision_point_back;
        //this should be possible to do in a nicer way...
        //wrap collision_point to other side
        Vector2 off;
        off.x = collision_point.x > bound.width/2 ? 10.f : -10.f;
        off.y = collision_point.y > bound.height/2 ? 10.f : -10.f;
        collision_point_back = collision_point+off;
        collision_point_back = mod(collision_point_back, {bound.width, bound.height});
        collision_point_back = collision_point_back-off;

        {
          Vector2 alt_attract = collision_point_back - collision_dist*player.data.orientation;

          p1 = alt_attract + Vector2Rotate(player.data.orientation,-cone_angle)*line_len;
          p2 = alt_attract + Vector2Rotate(player.data.orientation,cone_angle)*line_len;

          DrawLineEx(alt_attract, p1, 1, RED);
          DrawLineEx(alt_attract, p2, 1, RED);

          A = p1 - alt_attract;
          B = asteroids[ii].data.position - alt_attract;
          C = p2 - alt_attract;
          if (Vector2Length(asteroids[ii].data.position - alt_attract) < attract_distance
              && Vector2Cross(A,B) * Vector2Cross(A,C) >= 0
              && Vector2Cross(C,B) * Vector2Cross(C,A) >= 0)
          {
            Rectangle asteroid_bound;
            float margin = 10;
            asteroid_bound.x = asteroids[ii].data.position.x - asteroids[ii].data.radius - margin;
            asteroid_bound.y = asteroids[ii].data.position.y - asteroids[ii].data.radius - margin;
            asteroid_bound.width = 2*(asteroids[ii].data.radius + margin);
            asteroid_bound.height = 2*(asteroids[ii].data.radius + margin);
            DrawRectangleLinesEx(asteroid_bound, 2, RED);
            asteroids[ii].target = 2;
            asteroids[ii].attract_point = alt_attract;
          }
        }
      }
    }
  }

}

void suckAttack(const Vector2 &position, const Vector2& rotation, SuckAttack &suckAttack)
{
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
    suckAttack.balls.push_back(ball);

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

void gunUpdate(PlayerState& player, GunAttack &gun, std::vector<Shoot> &shoots)
{
  //update rotation
  Vector2 mousePointer = GetMousePosition();
  gun.direction = Vector2Normalize(mousePointer - player.data.position);

  //handle shooting
  if( IsMatchingKeyDown(options.keys[(size_t)GameOptions::ControlKeyCodes::FIRE]) && gun.cooldownTimer.getElapsed() >= gun.cooldownDuration && hasEnoughEnergy(player.energy, gun.energyCost) && player.storedAsteroids == 0){
    //FireShoot(player.data.position, gun.direction, player.movement.velocity, player.movement.maxAcceleration, shoots);
    FireShoot(player.data, gun.direction,500, shoots);
    if(options.sound_fx)
    {
      PlaySound(shoot_fx);
    }
    UpdateEnergy(player.energy, -gun.energyCost);
    gun.cooldownTimer.start();
  }
  if(IsMatchingKeyDown(options.keys[(size_t)GameOptions::ControlKeyCodes::FIRE]) && player.storedAsteroids > 0)
  {
    FireShootgun(player.data, player.storedAsteroids, gun.direction,500, shoots);
    gun.cooldownTimer.start();
  }
}

void laserUpdate(PlayerState &player){

  if (IsMatchingKeyPressed(options.keys[(size_t)GameOptions::ControlKeyCodes::ULTRA])) // && hasEnoughEnergy(player.laserEnergy, player.laserEnergy.maxValue))
  {
    OnStart(player.laser, player.gun.direction, player.data.position);
    player.laserEnergy.value = 0.f;
  }

  if(player.laser.isOngoing){
    Update(player.laser, player.gun.direction, player.data.position);
  }
}

void turnAsteroidToEnergy(PlayerState& player){
  if(player.storedAsteroids < 1){
    return;
  }

  player.storedAsteroids -= 1;
  float energyRefill = player.energy.maxValue / MAX_STORED_ASTEROIDS;
  UpdateEnergy(player.energy, energyRefill);
}

////////////////////////////////////////////////
///         Helper                          ///
///////////////////////////////////////////////
void rotateTriangle(Vector2 (&v)[3], const float angle)
{
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

bool hasEnoughEnergy(const Energy& energy, const float cost){
  return energy.value >= cost;
}

////////////////////////////////////////////////
///         Paint                            ///
///////////////////////////////////////////////
void DrawShip(const PlayerState &player)
{
  Vector2 bound = {(float)options.screenWidth, (float)options.screenWidth};
  Vector2 pos = player.data.position;
  Vector2 vertices[3];
  vertices[0] = pos + Vector2{15.f, 0.f};
  vertices[1] = {pos.x - 15.f, pos.y - 10.f};
  vertices[2] = {pos.x - 15.f, pos.y + 10.f};
  rotateTriangle(vertices, player.data.orientation, pos);

  //DrawTriangle(vertices[0],vertices[1],vertices[2],GREEN);

 float r = player.data.radius;
 float x = pos.x;
 float y = pos.y;

 Texture2D te = TEXTURES[0];
 // Source rectangle (part of the texture to use for drawing)
 Rectangle sourceRec = { 0.0f, 0.0f, (float)te.width, (float)te.height };

 // Destination rectangle (screen rectangle where drawing part of texture)
 Rectangle destRec = { x, y, 2*r, 2*r };
 // Origin of the texture (rotation/scale point), it's relative to destination rectangle size
 Vector2 origin = { r,r };

 int rotation = atan2(player.data.orientation.y,player.data.orientation.x)*180/M_PI + 90;
 DrawTexturePro(te, sourceRec, destRec, origin, (float)rotation, WHITE);

 if(player.dash_in_progress)
 {
   Vector2 dash_pos = player.data.position + player.data.orientation*options.screenWidth*DASH_DISTANCE;
   dash_pos = mod(dash_pos, {(float)options.screenWidth,(float)options.screenHeight});
   destRec.x = dash_pos.x;
   destRec.y = dash_pos.y;
   Color ghost = WHITE;
   ghost.a = 127;
   DrawTexturePro(te, sourceRec, destRec, origin, (float)rotation, ghost);
 }
}

void DrawGun(const PlayerState& player)
{
  Vector2 bound = {(float)options.screenWidth, (float)options.screenWidth};

  DrawCircleV(player.data.position, 5,RED);

  DrawLineEx(player.data.position, player.data.position + 10*player.gun.direction ,3, GRAY);

  float r = player.data.radius/2;
  float x = player.data.position.x;
  float y = player.data.position.y;

  Texture2D te = TEXTURES[1];
  // Source rectangle (part of the texture to use for drawing)
  Rectangle sourceRec = { 0.0f, 0.0f, (float)te.width, (float)te.height };

  // Destination rectangle (screen rectangle where drawing part of texture)
  Rectangle destRec = { x, y, 2*r, 2*r };
  // Origin of the texture (rotation/scale point), it's relative to destination rectangle size
  Vector2 origin = { r,r };

  int rotation = atan2(player.gun.direction.y,player.gun.direction.x)*180/M_PI + 90;
  DrawTexturePro(te, sourceRec, destRec, origin, (float)rotation, WHITE);
}

void PlayerState::OnHit()
{
  if(!options.godMode)
  {
    alive = false;
  }
}

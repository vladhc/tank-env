#include <iostream>
#include <math.h>
#include "box2d/box2d.h"
#include "tank.h"
#include "geom.h"
#include "game_object.h"
#include "bullet.h"

const float MAX_ANGULARY_VELOCITY = 1.0f;
const float ANGLE_TORQUE = 800.0f;

const float MAX_VELOCITY = 22.0f;
const double ACCELERATION = 800.0f;

const double FIRE_RANGE = 5.0;
const double VISION_RANGE = 10.0;
const int MAX_FIRE_COOLDOWN = 5;
const int MAX_HITPOINTS = 100;
const float WIDTH = 5.92f;

Tank::Tank(b2World* world, b2Vec2 position, float angle) :
    hitpoints(MAX_HITPOINTS),
    fire_cooldown_(0),
    GameObject(TANK)
{
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position = position;
  bodyDef.angle = angle;

  body_ = world->CreateBody(&bodyDef);
  body_->SetUserData(this);

  b2PolygonShape tankShape;
  tankShape.SetAsBox(WIDTH, 3.0f);

  body_->CreateFixture(&tankShape, 1.0f);
}

Tank::~Tank() {
  body_->GetWorld()->DestroyBody(body_);
}

float Tank::GetAngle() {
  return body_->GetAngle();
}

b2Vec2 Tank::GetPosition() {
  return body_->GetPosition();
}

float Tank::GetSize() {
  return WIDTH;
}

b2Body* Tank::GetBody() {
  return body_;
}

float min(float a, float b) {
  if (a < b) {
    return a;
  }
  return b;
}

float max(float a, float b) {
  if (a > b) {
    return a;
  }
  return b;
}

void Tank::Drive(float anglePower, float power) {
  if (!IsAlive()) {
    return;
  }
  // Turn if needed
  float angularVelocity = body_->GetAngularVelocity();
  if (anglePower != 0.0f && abs(angularVelocity) < MAX_ANGULARY_VELOCITY) {
    // Normalize anglePower
    anglePower = min(anglePower, 1.0f);
    anglePower = max(anglePower, -1.0f);

    float torque = anglePower * ANGLE_TORQUE;
    body_->ApplyAngularImpulse(torque, true);
  }

  // accelerate if needed
  b2Vec2 velocity = body_->GetLinearVelocity();
  if (power != 0 && velocity.Length() < MAX_VELOCITY) {
    power = min(power, 1.0f);
    power = max(power, -1.0f);

    float acceleration = ACCELERATION * power;
    b2Vec2 frontVec = body_->GetWorldVector(b2Vec2(1, 0));
    body_->ApplyLinearImpulseToCenter(
        b2Vec2(
          frontVec.x * acceleration,
          frontVec.y * acceleration
        ),
        true
    );
  }

  if (fire_cooldown_ > 0) {
    fire_cooldown_--;
  }
}

Bullet* Tank::Fire() {
  if (fire_cooldown_ > 0 || !IsAlive()) {
    return NULL;
  }
  fire_cooldown_ += MAX_FIRE_COOLDOWN;
  b2Vec2 pos = body_->GetWorldVector(b2Vec2(WIDTH + 0.15f, 0));
  pos += body_->GetPosition();
  return new Bullet(
      body_->GetWorld(),
      pos,
      body_->GetWorldVector(b2Vec2(3000.0f, 0)),
      this
  );
}

void Tank::TakeDamage(Bullet* bullet) {
  hitpoints = max(0, hitpoints - 30);
}

bool Tank::IsAlive() {
  return hitpoints > 0;
}

int Tank::GetHitpoints() {
  return hitpoints;
}

void printTank(Tank *tank) {
  std::cout << "<Tank";
  std::cout << " hitPoints=" << tank->GetHitpoints() << ";";
  b2Vec2 pos = tank->GetPosition();
  std::cout << " position=(" << pos.x << ", " << pos.y << ");";
  std::cout << " angle=" << tank->GetAngle() << "pi;";
  b2Vec2 velocity = tank->GetBody()->GetLinearVelocity();
  std::cout << " velocity=(" << velocity.x << ", " << velocity.y << "); ";
  std::cout << " angVelocity=" << tank->GetBody()->GetAngularVelocity() << "; ";
  std::cout << ">" << std::endl;
}

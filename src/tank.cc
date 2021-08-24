#include <iostream>
#include <math.h>
#include "tank.h"
#include "target.h"
#include "geom.h"
#include "box2d/box2d.h"

const float MAX_ANGULARY_VELOCITY = 1.0f;
const float ANGLE_TORQUE = 800.0f;

const float MAX_VELOCITY = 22.0f;
const double ACCELERATION = 800.0f;

const double FIRE_RANGE = 5.0;
const double VISION_RANGE = 10.0;
const int MAX_FIRE_COOLDOWN = 3;
const int MAX_HITPOINTS = 100;
const double SIZE = 5.92;
const Target NULL_TARGET = Target{b2Vec2(0.0, 0.0), false};

Tank::Tank(b2World* world) :
    hit_points_(MAX_HITPOINTS),
    speed_(0.0),
    fire_cooldown_(0),

    fire_target_(NULL_TARGET),
    move_target_(NULL_TARGET)
{
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(50.0f, 50.0f);

  body_ = world->CreateBody(&bodyDef);
  body_->SetUserData(this);

  b2PolygonShape tankShape;
  tankShape.SetAsBox(5.92f, 3.0f);

  body_->CreateFixture(&tankShape, 1.0f);
}

Tank::~Tank() {
  body_->GetWorld()->DestroyBody(body_);
}

void Tank::Stop(bool resetTarget) {
  speed_ = 0.;
  if (resetTarget && move_target_.is_active) {
    move_target_ = NULL_TARGET;
  }
}

float Tank::GetAngle() {
  return body_->GetAngle();
}

float Tank::GetSpeed() {
  return speed_;
}

b2Vec2 Tank::GetPosition() {
  return body_->GetPosition();
}

Target Tank::GetMoveTarget() {
  return move_target_;
}

void Tank::MoveTo(b2Vec2 coord) {
  move_target_ = Target{coord, true};
}

float Tank::GetSize() {
  return SIZE;
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
    // std::cout << "accelerating " << acceleration << std::endl;
    b2Vec2 frontVec = body_->GetWorldVector(b2Vec2(1, 0));
    body_->ApplyLinearImpulseToCenter(
        b2Vec2(
          frontVec.x * acceleration,
          frontVec.y * acceleration
        ),
        true
    );
    // }
  }
}

void printTank(Tank *tank) {
  std::cout << "<Tank";
  b2Vec2 pos = tank->GetPosition();
  std::cout << " position=(" << pos.x << ", " << pos.y << ");";
  std::cout << " angle=" << tank->GetAngle() << "pi;";
  Target target = tank->GetMoveTarget();
  std::cout << " speed=" << tank->GetSpeed() << "; ";

  std::cout << " angVelocity=" << tank->GetBody()->GetAngularVelocity() << "; ";

  std::cout << " moveTarget: (" << target.coord.x << ", " <<
                target.coord.y << ") is_active=" << target.is_active;
  std::cout << ">" << std::endl;
}

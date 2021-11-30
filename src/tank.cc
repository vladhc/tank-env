#include <iostream>
#include <math.h>
#include "box2d/box2d.h"
#include "tank.h"
#include "geom.h"
#include "game_object.h"
#include "bullet.h"

const float MAX_ANGULAR_VELOCITY = 1.2f;
const float ANGLE_TORQUE = 1.0f;

const float MAX_VELOCITY = 6.0f;
const double ACCELERATION = 10.0f;

const double FIRE_RANGE = 5.0;
const double VISION_RANGE = 10.0;
const float WIDTH = 1.5f;
const float HEIGHT = 0.75f;

Tank::Tank(int id, int teamId, b2World* world) :
    GameObject{TANK},
    id{id},
    teamId{teamId},
    hitpoints{MAX_HITPOINTS},
    fire_cooldown{0}
{
  // Body
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;

  body = world->CreateBody(&bodyDef);
  body->SetUserData(this);

  b2PolygonShape tankShape;
  tankShape.SetAsBox(WIDTH, HEIGHT);

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &tankShape;
  fixtureDef.density = 1.0f;
  fixtureDef.filter.categoryBits = 0x0001;
  fixtureDef.filter.maskBits = 0x0001 | 0x0002 | 0x0004;
  body->CreateFixture(&fixtureDef);

  // Turret
  b2BodyDef turretDef;
  turretDef.type = b2_dynamicBody;

  turret = world->CreateBody(&turretDef);
  turret->SetUserData(this);

  b2PolygonShape turretShape;
  turretShape.SetAsBox(WIDTH * 0.6, HEIGHT * 0.6);
  fixtureDef.shape = &turretShape;
  turret->CreateFixture(&fixtureDef);

  // Joint
  b2RevoluteJointDef jointDef;
  jointDef.bodyA = body;
  jointDef.bodyB = turret;
  jointDef.collideConnected = false;
  jointDef.enableMotor = true;
  jointDef.maxMotorTorque = ANGLE_TORQUE;
  jointDef.enableLimit = false;
  joint = (b2RevoluteJoint*) world->CreateJoint(&jointDef);
}

Tank::~Tank() {
  body->GetWorld()->DestroyBody(body);
}

void Tank::SetTransform(const b2Vec2& position, float angle) {
  SetTransform(position, angle, angle);
}

void Tank::SetTransform(const b2Vec2& position, float bodyAngle, float turretAngle) {
  body->SetTransform(position, bodyAngle);
  turret->SetTransform(position, turretAngle);
}

float Tank::GetAngle() const {
  return body->GetAngle();
}

float Tank::GetAngularVelocity() const {
  return body->GetAngularVelocity();
}

b2Vec2 Tank::GetPosition() const {
  return body->GetPosition();
}

b2Vec2 Tank::GetLocalPoint(const b2Vec2& globalPoint) const {
  return body->GetLocalPoint(globalPoint);
}

b2Vec2 Tank::GetTurretLocalPoint(const b2Vec2& globalPoint) const {
  return turret->GetLocalPoint(globalPoint);
}

b2Vec2 Tank::GetWorldVector(const b2Vec2& localVector) const {
  return body->GetWorldVector(localVector);
}

b2Vec2 Tank::GetLinearVelocity() const {
  return body->GetLinearVelocity();
}

float Tank::GetSize() const {
  return WIDTH;
}

b2Body* Tank::GetBody() {
  return body;
}

b2Body* Tank::GetTurret() {
  return turret;
}

float Tank::GetTurretAngle() const {
  return turret->GetAngle();
}

float Tank::GetTurretAngularVelocity() const {
  return turret->GetAngularVelocity();
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

void Tank::Drive(float anglePower, float turretAnglePower, float power) {
  if (!IsAlive()) {
    return;
  }

  // Turn if needed
  {
    float angularVelocity = body->GetAngularVelocity();
    if ((anglePower > 0 && angularVelocity < MAX_ANGULAR_VELOCITY) || (anglePower < 0 && angularVelocity > -MAX_ANGULAR_VELOCITY)) {
      auto target = anglePower > 0 ? MAX_ANGULAR_VELOCITY : -MAX_ANGULAR_VELOCITY;
      auto delta = (target - angularVelocity) * abs(anglePower);
      delta = min(delta, 1.0f);
      delta = max(delta, -1.0f);

      float torque = ANGLE_TORQUE * delta;
      body->ApplyAngularImpulse(torque, true);
    }
  }

  // Turn turret
  {
    turretAnglePower = min(turretAnglePower, 1.0f);
    turretAnglePower = max(turretAnglePower, -1.0f);
    joint->SetMotorSpeed(turretAnglePower);
  }

  // Accelerate
  {
    b2Vec2 v = body->GetLinearVelocity();
    b2Vec2 u = body->GetWorldVector(b2Vec2(1, 0));
    auto current = v.x * u.x + v.y * u.y;
    if ((power > 0 && current < MAX_VELOCITY) || (power < 0 && current > -MAX_VELOCITY)) {
      auto target = power > 0 ? MAX_VELOCITY : -MAX_VELOCITY;
      auto delta = (target - current) * abs(power);
      delta = min(delta, 1.0f);
      delta = max(delta, -1.0f);

      float acceleration = ACCELERATION * delta;
      body->ApplyLinearImpulseToCenter(
          b2Vec2(
            u.x * acceleration,
            u.y * acceleration
          ),
          true
      );
    }
  }

  if (fire_cooldown > 0) {
    fire_cooldown--;
  }
}

Bullet* Tank::Fire() {
  if (fire_cooldown > 0 || !IsAlive()) {
    return NULL;
  }
  fire_cooldown += MAX_FIRE_COOLDOWN;
  b2Vec2 pos = turret->GetWorldVector(b2Vec2(WIDTH, 0));
  pos += turret->GetPosition();
  return new Bullet(
      turret->GetWorld(),
      pos,
      turret->GetWorldVector(b2Vec2(1.0f, 0)),
      this
  );
}

unsigned int Tank::GetFireCooldown() const {
  return fire_cooldown;
}

void Tank::TakeDamage(unsigned int damage) {
  if (damage > hitpoints) {
    damage = hitpoints;
  }
  hitpoints -= damage;
  if (!IsAlive()) {
    joint->SetMotorSpeed(0);
  }
}

bool Tank::IsAlive() const {
  return hitpoints > 0;
}

unsigned int Tank::GetHitpoints() const {
  return hitpoints;
}

void Tank::Reset() {
  hitpoints = MAX_HITPOINTS;
  fire_cooldown = 0;
  Drive(0.0f, 0.0f, 0.0f);
  body->SetLinearVelocity(b2Vec2{0, 0});
  body->SetAngularVelocity(0);
  turret->SetAngularVelocity(0);
}

int Tank::GetId() const {
  return id;
}

int Tank::GetTeamId() const {
  return teamId;
}

void printTank(const Tank& tank) {
  std::cout << "<Tank";
  std::cout << " hitPoints=" << tank.GetHitpoints() << ";";
  b2Vec2 pos = tank.GetPosition();
  std::cout << " position=(" << pos.x << ", " << pos.y << ");";
  std::cout << " angle=" << tank.GetAngle() << "pi;";
  b2Vec2 velocity = tank.GetLinearVelocity();
  std::cout << " velocity=(" << velocity.x << ", " << velocity.y << "); ";
  std::cout << " angVelocity=" << tank.GetAngularVelocity() << "; ";
  std::cout << ">" << std::endl;
}

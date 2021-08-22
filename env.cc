#include <iostream>
#include <tuple>
#include <math.h>
#include <stdlib.h>
#include "box2d/box2d.h"
#include "tank.h"
#include "env.h"
#include "action.h"
#include "geom.h"

const float ARENA_SIZE = 100.0f;  // meters. w = h = 2 * ARENA_SIZE
const double PI = 3.14159265;
const double IS_AHEAD_THRESHOLD = PI / 4;
const double EPSILON = 0.0001;

const int VELOCITY_ITERATIONS = 6;
const int POSITION_ITERATIONS = 2;

Env::Env() {
  b2Vec2 gravity(0.0f, 0.0f);
  world_ = new b2World(gravity);
  tank_ = new Tank(world_);

  const float k_restitution = 0.1f;

  b2Body* ground;
  {
    b2BodyDef bd;
    bd.position.Set(0.0f, 0.0f);
    ground = world_->CreateBody(&bd);

    b2EdgeShape shape;

    b2FixtureDef sd;
    sd.shape = &shape;
    sd.density = 0.0f;
    sd.restitution = k_restitution;

    // Left vertical
    shape.SetTwoSided(
        b2Vec2(-ARENA_SIZE, -ARENA_SIZE),
        b2Vec2(-ARENA_SIZE, ARENA_SIZE));
    ground->CreateFixture(&sd);

    // Right vertical
    shape.SetTwoSided(
        b2Vec2(ARENA_SIZE, -ARENA_SIZE),
        b2Vec2(ARENA_SIZE, ARENA_SIZE));
    ground->CreateFixture(&sd);

    // Top horizontal
    shape.SetTwoSided(
        b2Vec2(-ARENA_SIZE, ARENA_SIZE),
        b2Vec2(ARENA_SIZE, ARENA_SIZE));
    ground->CreateFixture(&sd);

    // Bottom horizontal
    shape.SetTwoSided(
        b2Vec2(-ARENA_SIZE, -ARENA_SIZE),
        b2Vec2(ARENA_SIZE, -ARENA_SIZE));
    ground->CreateFixture(&sd);
  }

  {
    b2FrictionJointDef jd;
    jd.bodyA = ground;
    jd.bodyB = tank_->GetBody();
    jd.localAnchorA.SetZero();
    jd.localAnchorB = tank_->GetBody()->GetLocalCenter();
    jd.collideConnected = true;
    jd.maxForce = 6000.0f;
    jd.maxTorque = 6000.0f;

    world_->CreateJoint(&jd);
  }
}

Env::~Env() {
  delete tank_;
  delete world_;
}

bool isAhead(b2Vec2 src, b2Vec2 target, double curAngle) {
  double delta = angleDelta(src, target, curAngle);
  return abs2(delta) <= IS_AHEAD_THRESHOLD;
}

void moveTank(Tank *tank) {
  Target target = tank->GetMoveTarget();
  if (!target.is_active) {
    tank->Stop(true);
    return;
  }
  b2Vec2 pos = tank->GetPosition();
  float dist = calcDistance(pos, target.coord);
  if (dist <= EPSILON) {
    tank->Stop(true);
    return;
  }
  if (!isAhead(pos, target.coord, tank->GetAngle())) {
    tank->Stop(false);
    return;
  }

  // speed_ = std::min(speed_ + ACCELERATION, MAX_SPEED);
}

Observation createObservation(Tank* tank) {
  return Observation {
    tank,
    ARENA_SIZE
  };
}

Observation Env::Reset() {
  return createObservation(tank_);
}

std::tuple<Observation, double, bool> Env::Step(Action action) {
  tank_->Drive(action.anglePower, action.power);
  world_->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);

  Observation obs = createObservation(tank_);
  float reward = 0.1;
  bool done = false;

  return std::make_tuple(obs, reward, done);
}

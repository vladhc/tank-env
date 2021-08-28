#include <iostream>
#include <tuple>
#include <math.h>
#include <stdlib.h>
#include "box2d/box2d.h"
#include "tank.h"
#include "env.h"
#include "action.h"
#include "geom.h"
#include "strategic_point.h"
#include "collision_processor.h"

const float ARENA_SIZE = 20.0f;  // meters. w = h = 2 * ARENA_SIZE
const double PI = 3.14159265;
const double IS_AHEAD_THRESHOLD = PI / 4;
const double EPSILON = 0.0001;

const int VELOCITY_ITERATIONS = 24;
const int POSITION_ITERATIONS = 8;

Env::Env() {
  b2Vec2 gravity(0.0f, 0.0f);
  world_ = new b2World(gravity);

  const float k_restitution = 0.1f;

  b2Body* ground;
  {
    b2BodyDef bd;
    bd.type = b2_staticBody;
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

  int y_coords[] = {-15, -10, 0, 10, 15};
  int x_coords[] = {-15, 15};
  int idx = 0;
  for (const int y : y_coords) {
    for (const int x : x_coords) {
      float angle = (x > 0) ? -3.14 : 0;
      Tank* tank = new Tank(idx, x > 0, world_, b2Vec2(x, y), angle);
      b2FrictionJointDef jd;
      jd.bodyA = ground;
      jd.bodyB = tank->GetBody();
      jd.localAnchorA.SetZero();
      jd.localAnchorB = tank->GetBody()->GetLocalCenter();
      jd.collideConnected = true;
      jd.maxForce = 1500.0f;
      jd.maxTorque = 700.0f;

      world_->CreateJoint(&jd);
      tanks.push_back(tank);
      idx++;
    }
  }

  strategicPoint = new StrategicPoint(world_, b2Vec2(0.0f, 0.0f));

  collisionProcessor = new CollisionProcessor(world_);
}

Env::~Env() {
  for (Tank* tank : tanks) {
    delete tank;
  }
  for (Bullet* bullet : bullets) {
    delete bullet;
  }
  delete strategicPoint;
  delete collisionProcessor;
  delete world_;
}

std::vector<Observation> Env::CreateObservations() {
  std::vector<Observation> obs;
  for (Tank* hero : tanks) {
    std::vector<Tank*> allies;
    for (Tank* tank : tanks) {
      if (tank != hero) {
        allies.push_back(tank);
      }
    }
    obs.push_back(Observation {
      hero,
      allies,
      ARENA_SIZE,
      strategicPoint,
    });
  }
  return obs;
}

std::vector<Observation> Env::Reset() {
  return CreateObservations();
}

void Env::deleteBullet(Bullet* bullet) {
  for (auto it=bullets.begin(); it != bullets.end(); it++) {
    Bullet* curBullet = *it;
    if (curBullet == bullet) {
      bullets.erase(it);
      delete bullet;
      return;
    }
  }
}

std::tuple<
  std::vector<Observation>,
  std::vector<float>,
  std::vector<char>
> Env::Step(Action actions[]) {
  for (unsigned int i=0; i < tanks.size(); i++) {
    tanks[i]->Drive(actions[i].anglePower, actions[i].power);
    if (actions[i].fire) {
      Bullet* bullet = tanks[i]->Fire();
      if (bullet != NULL) {
        bullets.push_back(bullet);
      }
    }
  }
  world_->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
  TypedContact c;

  while (collisionProcessor->PollEvent(&c)) {
    if (c.bullet != NULL) {
      if (c.tank != NULL) {
        c.tank->TakeDamage(30);
      }
      deleteBullet(c.bullet);
    }
    if (c.point != NULL) {
      c.point->SetOwner(c.tank);
    }
  }

  std::vector<Observation> obs = CreateObservations();

  std::vector<float> rewards;
  std::vector<char> dones;

  for (Tank* tank : tanks) {
    float reward = 0.0f;
    bool done = false;
    if (strategicPoint->GetOwner() == tank) {
      reward += 1.0f;
    }
    if (tank->GetHitpoints() == 0) {
      done = true;
    }
    rewards.push_back(reward);
    dones.push_back(done);
  }

  return std::make_tuple(obs, rewards, dones);
}

std::vector<Tank*> Env::GetTanks() {
  return tanks;
}

std::vector<Bullet*> Env::GetBullets() {
  return bullets;
}

StrategicPoint* Env::GetStrategicPoint() {
  return strategicPoint;
}

float Env::GetArenaSize() {
  return ARENA_SIZE;
}

bool Env::EpisodeComplete() {
  bool teamAlive[] = {false, false};
  for (Tank* tank : tanks) {
    if (!tank->IsAlive()) {
      continue;
    }
    teamAlive[tank->GetTeamId()] = true;
  }
  return !teamAlive[0] || !teamAlive[1];
}

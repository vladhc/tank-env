#include <iostream>
#include <tuple>
#include <map>
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
const int TANKS_COUNT = 10;

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

  for (unsigned int idx=0; idx < TANKS_COUNT; idx++) {
    Tank* tank = new Tank(idx, idx % 2 == 0, world_);
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
    alivePrevStep.push_back(true);
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
    int id = hero->GetId();
    bool alive = hero->IsAlive();
    if (!alive && !alivePrevStep[id]) {
      continue;
    }
    alivePrevStep[id] = alive;

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
  while (!bullets.empty()) {
    deleteBullet(bullets[0]);
  }

  // Reset tanks position and angle
  int yCoords[] = {-15, -10, 0, 10, 15};
  int teamIds[] = {0, 1};

  int idx = 0;
  for (const int teamId : teamIds) {
    const int x = teamId == 0 ? 15 : -15;
    const float angle = (x > 0) ? M_PI : 0;

    unsigned int yCoordIdx = 0;
    for (Tank* tank : GetTanks()) {
      if (tank->GetTeamId() != teamId) {
        continue;
      }
      const int y = yCoords[yCoordIdx];
      tank->GetBody()->SetTransform(b2Vec2(x, y), angle);
      yCoordIdx++;
    }
  }

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
> Env::Step(std::map<int, Action> actions) {

  // Apply actions
  for (const auto &x : actions) {
    int tankId = x.first;
    Tank* tank = tanks[tankId];
    if (!tank->IsAlive()) {
      continue;
    }
    Action action = x.second;
    tank->Drive(action.anglePower, action.power);
    if (action.fire) {
      Bullet* bullet = tank->Fire();
      if (bullet != NULL) {
        bullets.push_back(bullet);
      }
    }
  }
  world_->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
  TypedContact c;

  // Process collisions
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

  // Evaluate per-team reward
  float teamRewards[] = {0.0f, 0.0f};
  for (const Observation &obs : obs) {
    Tank* tank = obs.hero;
    const int teamId = tank->GetTeamId();
    const bool alive = tank->IsAlive();
    if (strategicPoint->GetOwner() == tank) {
      teamRewards[teamId] += 0.1f;
    }
    if (!tank->IsAlive()) {
      teamRewards[teamId] -= 1.0f;
    }
  }

  // Compose rewards, dones
  std::vector<float> rewards;
  std::vector<char> dones;

  for (const Observation &obs : obs) {
    Tank* tank = obs.hero;
    float reward = teamRewards[tank->GetTeamId()];
    rewards.push_back(reward);
    dones.push_back(!tank->IsAlive());
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

#include <iostream>
#include <tuple>
#include <map>
#include <random>
#include <functional>
#include <stdlib.h>
#include "box2d/box2d.h"
#include "tank.h"
#include "env.h"
#include "action.h"
#include "geom.h"
#include "strategic_point.h"
#include "collision_processor.h"

const float ARENA_SIZE = 20.0f;  // meters. w = h = 2 * ARENA_SIZE

const int VELOCITY_ITERATIONS = 24;
const int POSITION_ITERATIONS = 8;
const int TANKS_COUNT = 10;

class BodyCheckerCallback : public b2QueryCallback {
  public:
    bool ReportFixture(b2Fixture* fixture) {
      foundBodies = true;
      return false;
    }
    void Reset() { foundBodies = false; }
    bool FoundBodies() const { return foundBodies; }
  private:
    bool foundBodies;
};

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

std::vector<Observation> Env::CreateObservations() const {
  std::vector<Observation> obs;
  for (const Tank* hero : tanks) {
    auto heroId = hero->GetId();
    if (!alivePrevStep[heroId]) {
      continue;
    }
    obs.push_back(Observation {
      heroId,
      GetTanks(),
      ARENA_SIZE,
      strategicPoint,
      GetBullets(),
    });
  }
  return obs;
}

std::vector<Observation> Env::Reset() {
  while (!bullets.empty()) {
    deleteBullet(bullets[0]);
  }

  static std::mt19937 random_engine;
  static auto angle_gen = std::bind(
      std::uniform_real_distribution<float> {0, 2 * M_PI},
      random_engine
  );
  static auto coord_gen = std::bind(
      std::uniform_real_distribution<float> {-ARENA_SIZE, ARENA_SIZE},
      random_engine
  );
  BodyCheckerCallback bodyCheckerCallback;

  for (Tank* tank : tanks) {
    const float angle = angle_gen();
    const float margin = tank->GetSize() + tank->GetSize() * 0.5;

    b2Vec2 pos;
    do {
      pos = b2Vec2{coord_gen(), coord_gen()};
      b2AABB aabbQuery;
      aabbQuery.lowerBound = b2Vec2{pos.x - margin, pos.y - margin};
      aabbQuery.upperBound = b2Vec2{pos.x + margin, pos.y + margin};

      bodyCheckerCallback.Reset();
      world_->QueryAABB(&bodyCheckerCallback, aabbQuery);
    } while(bodyCheckerCallback.FoundBodies());

    tank->GetBody()->SetTransform(pos, angle);
    tank->GetTurret()->SetTransform(pos, angle);

    const int id = tank->GetId();
    alivePrevStep[id] = true;
    tank->ResetHitpoints();
  }

  strategicPoint->SetOwner(NULL);

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
> Env::Step(const std::map<int, Action> actions) {

  // Apply actions
  for (const auto &x : actions) {
    int tankId = x.first;
    Tank* tank = tanks[tankId];
    if (!tank->IsAlive()) {
      continue;
    }
    Action action = x.second;
    tank->Drive(action.anglePower, action.turretAnglePower, action.power);
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
  std::map<int, float> perTankReward;
  for (int idx=0; idx < TANKS_COUNT; idx++) {
    perTankReward[idx] = 0.0f;
  }
  while (collisionProcessor->PollEvent(&c)) {
    if (c.bullet != NULL) {
      if (c.tank != NULL && c.tank->IsAlive()) {
        DamageTank(c.tank->GetId(), 30);
        const Tank* owner = (Tank*)c.bullet->GetOwner();
        const int ownerId = owner->GetId();
        if (owner->GetTeamId() == c.tank->GetTeamId()) {
          // friendly fire
          perTankReward[ownerId] -= 0.3f;
        } else {
          perTankReward[ownerId] += 0.3f;
        }
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
  for (const Observation &ob : obs) {
    const Tank* tank = ob.tanks[ob.heroId];
    const int teamId = tank->GetTeamId();
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

  for (const Observation &ob : obs) {
    const int tankId = ob.heroId;
    const Tank* tank = ob.tanks[tankId];
    float reward = perTankReward[tankId];
    reward += teamRewards[tank->GetTeamId()];
    rewards.push_back(reward);
    dones.push_back(!tank->IsAlive());
  }

  for (const Tank* tank : tanks) {
    const int id = tank->GetId();
    alivePrevStep[id] = tank->IsAlive();
  }

  return std::make_tuple(obs, rewards, dones);
}

std::vector<const Tank*> Env::GetTanks() const {
  std::vector<const Tank*> tanksImmutable;
  for (const Tank* tank : tanks) {
    tanksImmutable.push_back(tank);
  }
  return tanksImmutable;
}

void Env::DamageTank(int tankId, unsigned int damage) {
  auto tank = tanks[tankId];
  tank->TakeDamage(damage);
}

std::vector<const Bullet*> Env::GetBullets() const {
  std::vector<const Bullet*> bulletsImmutable;
  for (const Bullet* bullet : bullets) {
    bulletsImmutable.push_back(bullet);
  }
  return bulletsImmutable;
}

const StrategicPoint* Env::GetStrategicPoint() const {
  return strategicPoint;
}

float Env::GetArenaSize() const {
  return ARENA_SIZE;
}

bool Env::EpisodeComplete() const {
  bool teamAlive[] = {false, false};
  for (const Tank* tank : tanks) {
    if (!tank->IsAlive()) {
      continue;
    }
    teamAlive[tank->GetTeamId()] = true;
  }
  return !teamAlive[0] || !teamAlive[1];
}

#include <iostream>
#include <tuple>
#include <map>
#include <random>
#include <functional>
#include <stdlib.h>
#include <unistd.h>
#include "box2d/box2d.h"
#include "tank.h"
#include "lidar.h"
#include "bullet.h"
#include "env.h"
#include "action.h"
#include "geom.h"
#include "strategic_point.h"
#include "collision_processor.h"

const float ARENA_SIZE = 20.0f;  // meters. w = h = 2 * ARENA_SIZE
const float OBSTACLE_W = 5.0f;
const float OBSTACLE_H = 0.5f;

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

Env::Env(unsigned int tanksCount, unsigned int lidarRaysCount, size_t obstaclesCount) {
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
    sd.filter.categoryBits = 0x0004;
    sd.filter.maskBits = 0x0001;
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

  for (unsigned int idx=0; idx < tanksCount; idx++) {
    Tank* tank = new Tank(idx, idx % 2 == 0, world_);
    tank->AttachToGround(ground, world_);
    auto lidar = new Lidar(world_, tank->GetBody(), 5 * ARENA_SIZE, lidarRaysCount);
    tank->SetLidar(lidar);
    tanks.push_back(tank);
    alivePrevStep.push_back(true);
  }

  for (size_t idx=0; idx < obstaclesCount; idx++) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    auto obstacle = world_->CreateBody(&bodyDef);

    b2PolygonShape obstacleShape;
    obstacleShape.SetAsBox(OBSTACLE_W, OBSTACLE_H);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &obstacleShape;
    fixtureDef.density = 1.0f;
    fixtureDef.filter.categoryBits = 0x0004;
    fixtureDef.filter.maskBits = 0x0001 | 0x0002 | 0x0004;
    obstacle->CreateFixture(&fixtureDef);

    obstacles.push_back(obstacle);
  }

  // strategicPoint = new StrategicPoint(world_, b2Vec2(0.0f, 0.0f));

  collisionProcessor = new CollisionProcessor(world_);
}

Env::~Env() {
  for (Tank* tank : tanks) {
    delete tank;
  }
  for (Bullet* bullet : bullets) {
    delete bullet;
  }
  for (b2Body* obstacle : obstacles) {
    obstacle->GetWorld()->DestroyBody(obstacle);
  }
  // delete strategicPoint;
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
      // strategicPoint,
      GetBullets(),
      GetObstacles()
    });
  }
  return obs;
}

std::vector<Observation> Env::Reset() {
  while (!bullets.empty()) {
    deleteBullet(bullets[0]);
  }

  static std::mt19937 randomEngine(getpid());
  static auto angleGen = std::bind(
      std::uniform_real_distribution<float> {0, 2 * M_PI},
      randomEngine
  );
  static auto coordGen = std::bind(
      std::uniform_real_distribution<float> {-ARENA_SIZE, ARENA_SIZE},
      randomEngine
  );
  BodyCheckerCallback bodyCheckerCallback;

  for (b2Body* obstacle : obstacles) {
    const float angle = angleGen();
    obstacle->SetTransform(b2Vec2{coordGen(), coordGen()}, angle);
  }


  for (Tank* tank : tanks) {
    const float angle = angleGen();
    const float margin = tank->GetSize() + tank->GetSize() * 0.5;

    b2Vec2 pos;
    b2AABB aabbQuery;
    do {
      pos.x = coordGen();
      pos.y = coordGen();
      aabbQuery.lowerBound = b2Vec2{pos.x - margin, pos.y - margin};
      aabbQuery.upperBound = b2Vec2{pos.x + margin, pos.y + margin};

      bodyCheckerCallback.Reset();
      world_->QueryAABB(&bodyCheckerCallback, aabbQuery);
    } while(bodyCheckerCallback.FoundBodies());

    tank->SetTransform(pos, angle);

    const int id = tank->GetId();
    alivePrevStep[id] = true;
    tank->Reset();
  }

  // strategicPoint->SetOwner(NULL);

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

StepResult Env::Step(const std::map<int, Action> actions) {

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
  for (unsigned int idx=0; idx < tanks.size(); idx++) {
    // This will reward tanks to finish an episode earlier
    perTankReward[idx] = -0.001f;
  }
  // A bullet can hit multiple targets in one tick.
  // That's why we separate steps "process a bullet hit"
  // and "remove a bullet".
  std::vector<Bullet*> bulletsToDelete;
  while (collisionProcessor->PollEvent(&c)) {
    if (c.bullet != NULL) {
      if (c.tank != NULL && c.tank->IsAlive()) {

        // Inflict damage and punish the tank being hit
        const auto beingHit = c.tank;
        const auto beingHitId = beingHit->GetId();
        DamageTank(beingHitId, BULLET_DAMAGE);

        if (!beingHit->IsAlive()) {
          // Grant reward to attacker
          const Tank* attacker = (Tank*)c.bullet->GetOwner();
          const int attackerId = attacker->GetId();
          if (attacker->GetTeamId() == beingHit->GetTeamId()) {
            // friendly fire
            perTankReward[attackerId] -= 1.f;
          } else {
            perTankReward[attackerId] += 1.f;
          }
        }
      }
      bulletsToDelete.push_back(c.bullet);
    }
    if (c.point != NULL) {
      c.point->SetOwner(c.tank);
    }
  }
  for (auto bullet : bulletsToDelete) {
    deleteBullet(bullet);
  }

  std::vector<Observation> obs = CreateObservations();

  // Evaluate per-team reward
  float teamRewards[] = {0.0f, 0.0f};
  for (const Observation &ob : obs) {
    const Tank* tank = ob.tanks[ob.heroId];
    const int teamId = tank->GetTeamId();
    /*if (strategicPoint->GetOwner() == tank) {
      teamRewards[teamId] += 0.1f;
    }*/
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

  return StepResult{obs, rewards, dones};
}

std::vector<const Tank*> Env::GetTanks() const {
  std::vector<const Tank*> tanksImmutable;
  for (const Tank* tank : tanks) {
    tanksImmutable.push_back(tank);
  }
  return tanksImmutable;
}

std::vector<const b2Body*> Env::GetObstacles() const {
  std::vector<const b2Body*> obstaclesImmutable;
  for (const b2Body* obstacle : obstacles) {
    obstaclesImmutable.push_back(obstacle);
  }
  return obstaclesImmutable;
}

void Env::DamageTank(int tankId, unsigned int damage) {
  auto tank = tanks[tankId];
  tank->TakeDamage(damage);
}

void Env::SetTransform(int tankId, const b2Vec2& pos, float bodyAngle, float turretAngle) {
  auto tank = tanks[tankId];
  tank->SetTransform(pos, bodyAngle, turretAngle);
}

std::vector<const Bullet*> Env::GetBullets() const {
  std::vector<const Bullet*> bulletsImmutable;
  for (const Bullet* bullet : bullets) {
    const auto x = bullet->GetPosition().x;
    if (x < -ARENA_SIZE || x > ARENA_SIZE) {
      continue;
    }
    const auto y = bullet->GetPosition().y;
    if (y < -ARENA_SIZE || y > ARENA_SIZE) {
      continue;
    }
    bulletsImmutable.push_back(bullet);
  }
  return bulletsImmutable;
}

/*const StrategicPoint* Env::GetStrategicPoint() const {
  return strategicPoint;
}*/

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

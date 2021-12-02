#pragma once
#include <tuple>
#include <vector>
#include <map>
#include "box2d/box2d.h"
#include "action.h"
#include "tank.h"
#include "bullet.h"
#include "strategic_point.h"
#include "collision_processor.h"
#include "lidar.h"

const float TIME_STEP = 1.0f / 15.0f;
const int VELOCITY_ITERATIONS = 24;
const int POSITION_ITERATIONS = 8;

struct Observation {
  const int heroId;
  const std::vector<const Tank*> tanks;
  const float arenaSize;
  // const StrategicPoint* strategicPoint;
  const std::vector<const Bullet*> bullets;
};

class Env {
  public:
    Env(unsigned int tanksCount, unsigned int lidarRaysCount);
    ~Env();
    std::vector<Observation> Reset();
    std::tuple<
      std::vector<Observation>,
      std::vector<float>,
      std::vector<char>
    > Step(const std::map<int, Action> actions);
    std::vector<const Tank*> GetTanks() const;
    void DamageTank(int tankId, unsigned int damage);
    void SetTransform(int tankId, const b2Vec2& pos, float bodyAngle, float turretAngle);
    // const StrategicPoint* GetStrategicPoint() const;
    float GetArenaSize() const;
    std::vector<const Bullet*> GetBullets() const;
    bool EpisodeComplete() const;
  private:
    std::vector<Observation> CreateObservations() const;
    std::vector<Tank*> tanks;
    std::vector<Bullet*> bullets;
    b2World* world_;
    // StrategicPoint* strategicPoint;
    CollisionProcessor* collisionProcessor;
    void deleteBullet(Bullet* bullet);
    std::vector<char> alivePrevStep; // tankId -> wasAlivePreviousStep
};

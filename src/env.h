#pragma once
#include <tuple>
#include <vector>
#include <map>
#include "box2d/box2d.h"
#include "action.h"
#include "tank.h"
#include "bullet.h"
#include "collision_processor.h"
#include "lidar.h"

const float TIME_STEP = 1.0f / 15.0f;
const int VELOCITY_ITERATIONS = 24;
const int POSITION_ITERATIONS = 8;

struct Observation {
  const int heroId;
  const std::vector<const Tank*> tanks;
  const float arenaSize;
  const std::vector<const Bullet*> bullets;
  const std::vector<const b2Body*> obstacles;
};

struct StepResult {
  std::vector<Observation> observations;
  std::vector<float> rewards;
  std::vector<char> dones;
};

class Env {
  public:
    Env(unsigned int tanksCount, unsigned int lidarRaysCount, size_t obstaclesCount);
    ~Env();
    std::vector<Observation> Reset();
    StepResult Step(const std::map<int, Action> actions);
    std::vector<const Tank*> GetTanks() const;
    std::vector<const b2Body*> GetObstacles() const;
    void DamageTank(int tankId, unsigned int damage);
    void SetTransform(int tankId, const b2Vec2& pos, float bodyAngle, float turretAngle);
    float GetArenaSize() const;
    std::vector<const Bullet*> GetBullets() const;
    bool EpisodeComplete() const;
  private:
    std::vector<Observation> CreateObservations() const;
    std::vector<Tank*> tanks;
    std::vector<Bullet*> bullets;
    std::vector<b2Body*> obstacles;
    b2World* world_;
    CollisionProcessor* collisionProcessor;
    void deleteBullet(Bullet* bullet);
    std::vector<char> alivePrevStep; // tankId -> wasAlivePreviousStep
};

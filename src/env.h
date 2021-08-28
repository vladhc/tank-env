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

const float TIME_STEP = 1.0f / 15.0f;

struct Observation {
  Tank* hero;
  std::vector<Tank*> allies;
  float arenaSize;
  StrategicPoint* strategicPoint;
};

class Env {
  public:
    Env();
    ~Env();
    std::vector<Observation> Reset();
    std::tuple<
      std::vector<Observation>,
      std::vector<float>,
      std::vector<char>
    > Step(std::map<int, Action> actions);
    std::vector<Tank*> GetTanks();
    StrategicPoint* GetStrategicPoint();
    float GetArenaSize();
    std::vector<Bullet*> GetBullets();
    bool EpisodeComplete();
  private:
    std::vector<Observation> CreateObservations();
    int tanksCount;
    std::vector<Tank*> tanks;
    std::vector<Bullet*> bullets;
    b2World* world_;
    StrategicPoint* strategicPoint;
    CollisionProcessor* collisionProcessor;
    void deleteBullet(Bullet* bullet);
    std::vector<char> alivePrevStep; // tankId -> wasAlivePreviousStep
};

void moveTank(Tank *tank);

void rotateTank(Tank *tank);

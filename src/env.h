#pragma once
#include <tuple>
#include <vector>
#include "tank.h"
#include "box2d/box2d.h"
#include "action.h"
#include "strategic_point.h"

const float TIME_STEP = 1.0f / 15.0f;

struct Observation {
  Tank* hero;
  std::vector<Tank*> allies;
  float arenaSize;
  StrategicPoint* strategicPoint;
};

class ContactListener : public b2ContactListener {
  public:
    ContactListener();
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
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
    > Step(Action actions[]);
    std::vector<Tank*> GetTanks();
    StrategicPoint* GetStrategicPoint();
    float GetArenaSize();
  private:
    std::vector<Observation> CreateObservations();
    int tanksCount;
    std::vector<Tank*> tanks;
    b2World* world_;
    StrategicPoint* strategicPoint;
    ContactListener* contactListener;
};

void moveTank(Tank *tank);

void rotateTank(Tank *tank);

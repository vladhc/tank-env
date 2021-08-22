#pragma once
#include <tuple>
#include "tank.h"
#include "box2d/box2d.h"
#include "action.h"
#include "strategic_point.h"

struct Observation {
  Tank* tank;
  float arenaSize;
  StrategicPoint* strategicPoint;
};

const float TIME_STEP = 1.0f / 60.0f;

class Env {
  public:
    Env();
    ~Env();
    Observation Reset();
    std::tuple<Observation, double, bool> Step(Action action);
  private:
    Observation CreateObservation();
    Tank* tank_;
    b2World* world_;
    StrategicPoint* strategicPoint;
};

void moveTank(Tank *tank);

void rotateTank(Tank *tank);

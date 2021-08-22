#pragma once
#include <tuple>
#include "tank.h"
#include "box2d/box2d.h"
#include "action.h"

struct Observation {
  Tank* tank;
  float arenaSize;
};

const float TIME_STEP = 1.0f / 60.0f;

class Env {
  public:
    Env();
    ~Env();
    Observation Reset();
    std::tuple<Observation, double, bool> Step(Action action);
  private:
    Tank* tank_;
    b2World* world_;
};

void moveTank(Tank *tank);

void rotateTank(Tank *tank);

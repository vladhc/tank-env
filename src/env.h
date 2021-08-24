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

const float TIME_STEP = 1.0f / 15.0f;

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
    Observation Reset();
    std::tuple<Observation, double, bool> Step(Action action);
  private:
    Observation CreateObservation();
    Tank* tank_;
    b2World* world_;
    StrategicPoint* strategicPoint;
    ContactListener* contactListener;
};

void moveTank(Tank *tank);

void rotateTank(Tank *tank);

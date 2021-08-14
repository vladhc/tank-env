#include <tuple>
#include "tank.h"

struct Observation {
  Tank tank;
};

struct Action {
  bool move;
  Point moveTarget;
};

class Env {
  public:
    Env();
    Env(Tank t);
    Observation Reset();
    std::tuple<Observation, double, bool> Step(Action action);
  private:
    Tank tank_;
};

void moveTank(Tank *tank);

void rotateTank(Tank *tank);

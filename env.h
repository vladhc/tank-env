#include <tuple>
#include "tank.h"

struct Observation {
  Tank tank;
};

class Env {
  public:
    Env();
    Observation Reset();
    std::tuple<Observation, double, bool> Step();
  private:
    Tank tank_;
};

void moveTank(Tank *tank);

void rotateTank(Tank *tank);

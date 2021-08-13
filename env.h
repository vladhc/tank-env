#include <tuple>
#include "tank.h"

struct Observation {
};

class Env {
  public:
    Env();
    Observation Reset();
    std::tuple<Observation, float, bool> Step();
  private:
    Tank tank_;
};

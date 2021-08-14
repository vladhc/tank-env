#include <tuple>
#include "tank.h"

class Env {
  public:
    Env();
    double Reset();
    std::tuple<double, double, bool> Step();
  private:
    Tank tank_;
};

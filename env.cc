#include <iostream>
#include <tuple>
#include <cmath>
#include <math.h>
#include <stdlib.h>
#include "point.h"
#include "tank.h"
#include "env.h"
#include "geom.h"

const double ARENA_SIZE = 10.;  // max-x, max-y
const double PI = 3.14159265;
const double IS_AHEAD_THRESHOLD = PI / 4;

Env::Env() :
  tank_(
      Point{0.0, 0.0}, // position
      PI / 2 // angle
  )
{
  tank_.MoveTo(Point{5., 0.});
}

bool isAhead(Point src, Point target, double curAngle) {
  double delta = angleDelta(src, target, curAngle);
  return std::abs(delta) <= IS_AHEAD_THRESHOLD;
}

void moveTank(Tank *tank) {
  Target target = tank->GetMoveTarget();
  if (!target.is_active) {
    tank->Stop(true);
    return;
  }
  Point pos = tank->GetPosition();
  float dist = calcDistance(pos, target.coord);
  if (dist <= tank->GetSize()) {
    tank->Stop(true);
    return;
  }
  if (!isAhead(pos, target.coord, tank->GetAngle())) {
    tank->Stop(false);
    return;
  }
  tank->MoveTick();
}

void rotateTank(Tank *tank) {
  Target target = tank->GetMoveTarget();
  if (!target.is_active) {
    return;
  }
  double delta = angleDelta(
      tank->GetPosition(),
      target.coord,
      tank->GetAngle());
  tank->Rotate(delta);
}

Observation createObservation(Tank tank) {
  return Observation {
    tank
  };
}

Observation Env::Reset() {
  return createObservation(tank_);
}

std::tuple<Observation, double, bool> Env::Step() {
  std::cout << "Env::Step(); Before" << std::endl;
  printTank(&tank_);

  moveTank(&tank_);
  rotateTank(&tank_);

  std::cout << "Env::Step(); After" << std::endl;
  printTank(&tank_);
  std::cout << "------------------" << std::endl;

  Observation obs = createObservation(tank_);
  float reward = 0.1;
  bool done = false;

  return std::make_tuple(obs, reward, done);
}

#include <iostream>
#include <tuple>
#include <cmath>
#include <math.h>
#include <stdlib.h>
#include "point.h"
#include "tank.h"
#include "env.h"

const float SIZE = 25.;  // max-x, max-y
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

double calcDistance(Point p1, Point p2) {
  double dx = p2.x - p1.x;
  double dy = p2.y - p1.y;
  return sqrt(dx * dx + dy * dy);
}

double angleDelta(Point src, Point target, double curAngle) {
  double dot = src.x * target.x + src.y * target.y;
  double det = src.x * target.y - src.y * target.x;
  double targetAngle = atan2(det, dot);
  if (targetAngle < 0) {
    targetAngle += 2 * PI;
  }
  if (curAngle < 0) {
    curAngle += 2 * PI;
  }
  return targetAngle - curAngle;
}

bool isAhead(Point src, Point target, double curAngle) {
  double delta = angleDelta(src, target, curAngle);
  return abs(delta) <= IS_AHEAD_THRESHOLD;
}

void moveTank(Tank *tank) {
  Point pos = tank->GetPosition();
  Target target = tank->GetMoveTarget();
  if (!target.is_active) {
    tank->Stop(false);
    return;
  }
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
    std::cout << "tank's target is inactive" << std::endl;
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

void printTank(Tank *tank) {
  std::cout << "Tank:" << std::endl;
  Point pos = tank->GetPosition();
  std::cout << "  Position: (" << pos.x << ", " << pos.y << ")" << std::endl;
  double angle = tank->GetAngle();
  std::cout << "  Angle: " << (angle / PI) << "pi" << std::endl;
  Target target = tank->GetMoveTarget();
  std::cout << "  MoveTarget: (" << target.coord.x << ", " <<
                target.coord.y << ") " << target.is_active << std::endl;
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

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

bool isAhead(Point src, Point target, double curAngle) {
  double dot = src.x * target.x + src.y * target.y;
  double det = src.x * target.y - src.y * target.x;
  double targetAngle = atan2(det, dot);
  if (targetAngle < 0) {
    targetAngle += 2 * PI;
  }
  if (curAngle < 0) {
    curAngle += 2 * PI;
  }
  std::cout << "target angle " << targetAngle << std::endl;
  std::cout << "cur angle " << curAngle << std::endl;
  std::cout << "threshold " << IS_AHEAD_THRESHOLD << std::endl;

  double delta = targetAngle - curAngle;
  return abs(delta) <= IS_AHEAD_THRESHOLD;
}

void moveTank(Tank *tank) {
  Point pos = tank->GetPosition();
  Target target = tank->GetMoveTarget();
  if (!target.is_active) {
    tank->Stop();
    return;
  }
  float dist = calcDistance(pos, target.coord);
  if (dist <= tank->GetSize()) {
    tank->Stop();
    return;
  }
  if (!isAhead(pos, target.coord, tank->GetAngle())) {
    tank->Stop();
    return;
  }
  tank->MoveTick();
}

void rotateTank(Tank *tank) {
}

Observation Env::Reset() {
  Observation obs{};
  return obs;
}

std::tuple<Observation, float, bool> Env::Step() {
  Point pos = tank_.GetPosition();
  std::cout << "Env::Step(); Position before: (" << pos.x << "," << pos.y << ")" << std::endl;
  moveTank(&tank_);
  pos = tank_.GetPosition();
  std::cout << "Env::Step(); Position after: (" << pos.x << "," << pos.y << ")" << std::endl;
  Observation obs{};
  float reward = 0.1;
  bool done = false;
  return std::make_tuple(obs, reward, done);
}

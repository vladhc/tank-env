#include <iostream>
#include <math.h>
#include <cmath>
#include "tank.h"
#include "point.h"
#include "target.h"
#include "geom.h"

const double MAX_SPEED = 0.15;
const double ACCELERATION = 0.02;
const double ANGLE_SPEED = 0.1;
const double FIRE_RANGE = 5.0;
const double VISION_RANGE = 10.0;
const int MAX_FIRE_COOLDOWN = 3;
const int MAX_HITPOINTS = 100;
const double SIZE = 0.3;
const Target NULL_TARGET = Target{Point{0.0, 0.0}, false};
const double PI = 3.14159265;
const double EPSILON = 0.00001;

Tank::Tank(
    Point position,
    double angle
) :
    hit_points_(MAX_HITPOINTS),
    speed_(0.0),
    angle_(angle),
    position_(position),
    fire_cooldown_(0),

    fire_target_(NULL_TARGET),
    move_target_(NULL_TARGET)
{
}

void Tank::Stop(bool resetTarget) {
  speed_ = 0.;
  if (resetTarget && move_target_.is_active) {
    move_target_ = NULL_TARGET;
  }
}

void Tank::MoveTick() {
  speed_ = std::min(speed_ + ACCELERATION, MAX_SPEED);
  double distance = calcDistance(move_target_.coord, position_);
  double to_travel = std::min(speed_, distance);
  position_ = Point{
    position_.x + cos(angle_) * to_travel,
    position_.y + sin(angle_) * to_travel
  };
}

void Tank::Rotate(double angle) {
  if (std::abs(angle) < EPSILON) {
    return;
  }
  speed_ = 0.;
  if (angle > 0) {
    angle = std::min(angle, ANGLE_SPEED);
  } else {
    angle = std::max(angle, -ANGLE_SPEED);
  }
  angle_ += angle;
}

float Tank::GetAngle() {
  return angle_;
}

float Tank::GetSpeed() {
  return speed_;
}

Point Tank::GetPosition() {
  return position_;
}

Target Tank::GetMoveTarget() {
  return move_target_;
}

void Tank::MoveTo(Point coord) {
  move_target_ = Target{coord, true};
}

float Tank::GetSize() {
  return SIZE;
}

void printTank(Tank *tank) {
  std::cout << "<Tank";
  Point pos = tank->GetPosition();
  std::cout << " position=(" << pos.x << ", " << pos.y << ");";
  double angle = tank->GetAngle();
  std::cout << " angle=" << (angle / PI) << "pi;";
  Target target = tank->GetMoveTarget();
  std::cout << " speed=" << tank->GetSpeed() << "; ";
  std::cout << " moveTarget: (" << target.coord.x << ", " <<
                target.coord.y << ") is_active=" << target.is_active;
  std::cout << ">" << std::endl;
}

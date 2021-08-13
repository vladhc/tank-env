#include <iostream>
#include "tank.h"
#include "point.h"
#include "target.h"
#include <math.h>

const double ACCELERATION = 0.1;
const double ANGLE_ACCELERATION = 0.1;
const double FIRE_RANGE = 5.0;
const double VISION_RANGE = 10.0;
const double MAX_SPEED = 1.0;
const int MAX_FIRE_COOLDOWN = 3;
const int MAX_HITPOINTS = 100;
const double SIZE = 0.3;
const Target NULL_TARGET = Target{Point{0.0, 0.0}, false};

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
  std::cout << "constructing a tank" << std::endl;
}

void Tank::Stop() {
  speed_ = 0.;
  move_target_ = NULL_TARGET;
}

void Tank::MoveTick() {
  std::cout << "Tank::MoveTick()" << std::endl;
  speed_ = std::min(speed_ + ACCELERATION, MAX_SPEED);
  position_ = Point{
    position_.x + cos(angle_) * speed_,
    position_.y + sin(angle_) * speed_
  };
}

void Tank::Rotate(double angle) {
  speed_ = 0.;
  angle_ += angle;
}

float Tank::GetAngle() {
  return angle_;
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

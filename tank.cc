#include <iostream>
#include "tank.h"
#include "point.h"
#include "target.h"

const float ACCELERATION = 0.1;
const float ANGLE_ACCELERATION = 0.1;
const float FIRE_RANGE = 5.0;
const float VISION_RANGE = 10.0;
const float MAX_SPEED = 1.0;
const int MAX_FIRE_COOLDOWN = 3;
const int MAX_HITPOINTS = 100;

Tank::Tank(
    Point position,
    float angle
) :
    hit_points_(MAX_HITPOINTS),
    speed_(0.0),
    angle_(angle),
    position_(position),
    fire_cooldown_(0),

    fire_target_(Target{Point{0.0, 0.0}, false}),
    move_target_(Target{Point{0.0, 0.0}, false})
{
  std::cout << "constructing a tank" << std::endl;
}

void Tank::Tick() {
  std::cout << "Tank.Tick()" << std::endl;
}

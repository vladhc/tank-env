#include "tank.h"
#include "point.h"
#include <iostream>

Tank::Tank(
    float max_speed,
    float acceleration,
    float angle_acceleration,
    float fire_range,
    float vision_range,
    int max_fire_cooldown,
    Point position,
    float angle,
    float hit_points
) :
    max_speed_(max_speed),
    acceleration_(acceleration),
    angle_acceleration_(angle_acceleration),
    fire_range_(fire_range),
    vision_range_(vision_range),
    max_fire_cooldown_(max_fire_cooldown),

    hit_points_(hit_points),
    speed_(0.0),
    angle_(angle),
    position_(position),
    fire_cooldown_(0),

    fire_target_(nullptr),
    move_target_(nullptr)
{
  std::cout << "constructing a tank" << std::endl;
}

void Tank::Tick() {
  std::cout << "Tank.Tick()" << std::endl;
}

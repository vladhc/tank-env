#pragma once
#include "point.h"

class Tank {
  public:
    Tank(
        float max_speed,
        float acceleration,
        float angle_acceleration,
        float fire_range,
        float vision_range,
        int max_fire_cooldown,
        Point position,
        float angle,
        float hit_points
    );
    void Tick();
  private:
    float max_speed_;
    float acceleration_;
    float angle_acceleration_;
    float fire_range_;
    float vision_range_;
    int max_fire_cooldown_;

    float hit_points_;
    float speed_;
    float angle_;
    Point position_;
    int fire_cooldown_;

    Point *fire_target_;
    Point *move_target_;
};

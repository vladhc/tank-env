#pragma once
#include "point.h"
#include "target.h"

class Tank {
  public:
    Tank(
        Point position,
        float angle
    );
    void Tick();
  private:
    int hit_points_;
    float speed_;
    float angle_;
    Point position_;
    int fire_cooldown_;

    Target fire_target_;
    Target move_target_;
};

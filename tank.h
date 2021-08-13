#pragma once
#include "point.h"
#include "target.h"

class Tank {
  public:
    Tank(
        Point position,
        double angle
    );
    void Stop(bool resetTarget);
    void MoveTick();
    void Rotate(double angle);
    float GetAngle();
    Point GetPosition();
    Target GetMoveTarget();
    void MoveTo(Point coord);
    float GetSize();
  private:
    int hit_points_;
    double speed_;
    double angle_;
    Point position_;
    int fire_cooldown_;

    Target fire_target_;
    Target move_target_;
};

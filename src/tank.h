#pragma once
#include "target.h"
#include "box2d/box2d.h"

class Tank {
  public:
    Tank(b2World* world, b2Vec2 position, float angle);
    ~Tank();
    void Stop(bool resetTarget);
    float GetAngle();
    float GetSpeed();
    b2Vec2 GetPosition();
    Target GetMoveTarget();
    void MoveTo(b2Vec2 coord);
    float GetSize();
    b2Body* GetBody();
    void Drive(float anglePower, float power);
  private:
    int hit_points_;
    double speed_;
    int fire_cooldown_;
    b2Body* body_;

    Target fire_target_;
    Target move_target_;
};

void printTank(Tank *tank);

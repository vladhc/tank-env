#pragma once
#include "box2d/box2d.h"

class Tank {
  public:
    Tank(b2World* world, b2Vec2 position, float angle);
    ~Tank();
    float GetAngle();
    b2Vec2 GetPosition();
    float GetSize();
    b2Body* GetBody();
    void Drive(float anglePower, float power);
  private:
    int hit_points_;
    int fire_cooldown_;
    b2Body* body_;
};

void printTank(Tank *tank);

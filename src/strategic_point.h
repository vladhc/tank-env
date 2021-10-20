#pragma once
#include "box2d/box2d.h"
#include "tank.h"

class StrategicPoint : public GameObject {
  public:
    StrategicPoint(b2World* world, b2Vec2 position);
    ~StrategicPoint();
    Tank* GetOwner();
    void SetOwner(Tank* tank);
    b2Vec2 GetPosition() const;
  private:
    b2Body* body;
    Tank* owner;
};

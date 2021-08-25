#pragma once
#include <vector>
#include "box2d/box2d.h"
#include "tank.h"
#include "strategic_point.h"
#include "bullet.h"

struct TypedContact {
  Tank* tank;
  StrategicPoint* point;
  Bullet* bullet;
};

class CollisionProcessor : public b2ContactListener {
  public:
    CollisionProcessor(b2World* world);
    ~CollisionProcessor();
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    bool PollEvent(TypedContact* contact);
  private:
    std::vector<TypedContact> contacts;
    std::vector<TypedContact>::iterator iter;
    bool createIterator;
};

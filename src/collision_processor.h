#pragma once
#include <vector>
#include "box2d/box2d.h"
#include "tank.h"
#include "bullet.h"

struct TypedContact {
  Tank* tank;
  Bullet* bullet;
};

class CollisionProcessor : public b2ContactListener, public b2ContactFilter {
  public:
    CollisionProcessor(b2World* world);
    ~CollisionProcessor();
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB);
    bool PollEvent(TypedContact* contact);
  private:
    std::vector<TypedContact> contacts;
    std::vector<TypedContact>::iterator iter;
    bool createIterator;
};

#pragma once
#include "box2d/box2d.h"

class CollisionProcessor : public b2ContactListener {
  public:
    CollisionProcessor(b2World* world);
    ~CollisionProcessor();
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    void Step();
};

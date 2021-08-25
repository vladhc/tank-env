#pragma once
#include "box2d/box2d.h"
#include "game_object.h"

class Bullet : public GameObject {
  public:
    Bullet(b2World* world, b2Vec2 firePosition, b2Vec2 fireDirection, GameObject* owner);
    ~Bullet();
    b2Body* GetBody();
    GameObject* GetOwner();
  private:
    b2Body* body;
    GameObject* owner;
};

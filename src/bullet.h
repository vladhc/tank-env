#pragma once
#include "box2d/box2d.h"
#include "game_object.h"

const unsigned int BULLET_DAMAGE = 40;

class Bullet : public GameObject {
  public:
    Bullet(b2World* world, b2Vec2 firePosition, b2Vec2 fireDirection, GameObject* owner);
    ~Bullet();
    b2Vec2 GetPosition() const;
    b2Vec2 GetLinearVelocity() const;
    GameObject* GetOwner();
  private:
    b2Body* body;
    GameObject* owner;
};

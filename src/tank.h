#pragma once
#include "box2d/box2d.h"
#include "game_object.h"
#include "bullet.h"

class Tank : public GameObject {
  public:
    Tank(int id, b2World* world, b2Vec2 position, float angle);
    ~Tank();
    float GetAngle();
    b2Vec2 GetPosition();
    float GetSize();
    b2Body* GetBody();
    void Drive(float anglePower, float power);
    Bullet* Fire();
    void TakeDamage(Bullet* bullet);
    bool IsAlive();
    int GetHitpoints();
    int GetId();
  private:
    int id;
    int hitpoints;
    int fire_cooldown_;
    b2Body* body_;
};

void printTank(Tank *tank);

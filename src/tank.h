#pragma once
#include "box2d/box2d.h"
#include "game_object.h"
#include "bullet.h"

class Tank : public GameObject {
  public:
    Tank(int id, int teamId, b2World* world);
    ~Tank();
    float GetAngle();
    b2Vec2 GetPosition();
    float GetSize();
    b2Body* GetBody();
    b2Body* GetTurret();
    void Drive(float anglePower, float turretAnglePower, float power);
    Bullet* Fire();
    int GetFireCooldown();
    void TakeDamage(int damage);
    bool IsAlive();
    int GetHitpoints();
    void ResetHitpoints();
    int GetId();
    int GetTeamId();
  private:
    int id;
    int teamId;
    int hitpoints;
    int fire_cooldown_;
    b2Body* body_;
    b2Body* turret;
    b2RevoluteJoint* joint;
};

void printTank(Tank *tank);

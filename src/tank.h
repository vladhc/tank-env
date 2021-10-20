#pragma once
#include "box2d/box2d.h"
#include "game_object.h"
#include "bullet.h"

class Tank : public GameObject {
  public:
    Tank(int id, int teamId, b2World* world);
    ~Tank();
    float GetAngle() const;
    float GetAngularVelocity() const;
    b2Vec2 GetPosition() const;
    b2Vec2 GetLinearVelocity() const;
    b2Vec2 GetLocalPoint(b2Vec2 globalPoint) const;
    float GetSize() const;
    b2Body* GetBody();
    b2Body* GetTurret();
    float GetTurretAngle() const;
    float GetTurretAngularVelocity() const;
    void Drive(float anglePower, float turretAnglePower, float power);
    Bullet* Fire();
    unsigned int GetFireCooldown() const;
    void TakeDamage(unsigned int damage);
    bool IsAlive() const;
    unsigned int GetHitpoints() const;
    void ResetHitpoints();
    int GetId() const;
    int GetTeamId() const;
  private:
    int id;
    int teamId;
    unsigned int hitpoints;
    unsigned int fire_cooldown_;
    b2Body* body_;
    b2Body* turret;
    b2RevoluteJoint* joint;
};

void printTank(const Tank& tank);

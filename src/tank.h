#pragma once
#include "box2d/box2d.h"
#include "game_object.h"
#include "bullet.h"
#include "lidar.h"

const unsigned int MAX_FIRE_COOLDOWN = 30;
const unsigned int MAX_HITPOINTS = 100;

class Tank : public GameObject {
  public:
    Tank(int id, int teamId, b2World* world);
    ~Tank();
    void SetTransform(const b2Vec2& position, float bodyAngle);
    void SetTransform(const b2Vec2& position, float bodyAngle, float turretAngle);
    float GetAngle() const;
    float GetAngularVelocity() const;
    b2Vec2 GetPosition() const;
    b2Vec2 GetLinearVelocity() const;
    b2Vec2 GetLocalPoint(const b2Vec2& globalPoint) const;
    b2Vec2 GetTurretLocalPoint(const b2Vec2& globalPoint) const;
    b2Vec2 GetWorldVector(const b2Vec2& localVector) const;
    float GetSize() const;
    const b2Body* GetBody() const;
    const b2Body* GetTurret() const;
    float GetTurretAngle() const;
    float GetTurretAngularVelocity() const;
    void Drive(float anglePower, float turretAnglePower, float power);
    Bullet* Fire();
    unsigned int GetFireCooldown() const;
    void TakeDamage(unsigned int damage);
    bool IsAlive() const;
    unsigned int GetHitpoints() const;
    void Reset();
    int GetId() const;
    int GetTeamId() const;
    void AttachToGround(b2Body* ground, b2World* world);
    void SetLidar(const Lidar* lidar);
    const Lidar* GetLidar() const;
  private:
    int id;
    int teamId;
    unsigned int hitpoints;
    unsigned int fire_cooldown;
    b2Body* body;
    b2Body* turret;
    b2RevoluteJoint* joint;
    const Lidar* lidar;
};

void printTank(const Tank& tank);

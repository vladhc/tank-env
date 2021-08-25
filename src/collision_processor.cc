#include "box2d/box2d.h"
#include "tank.h"
#include "collision_processor.h"
#include "strategic_point.h"

CollisionProcessor::CollisionProcessor(b2World* world) {
  world->SetContactListener(this);
}

CollisionProcessor::~CollisionProcessor() {
}

void CollisionProcessor::Step() {
}

StrategicPoint* GetStrategicPoint(b2Contact* contact) {
  b2Fixture* fixtureA = contact->GetFixtureA();
  if (fixtureA->IsSensor()) {
    return (StrategicPoint*)( fixtureA->GetBody()->GetUserData() );
  }
  b2Fixture* fixtureB = contact->GetFixtureB();
  if (fixtureB->IsSensor()) {
    return (StrategicPoint*)( fixtureB->GetBody()->GetUserData() );
  }
  return NULL;
}

Tank* GetTank(b2Contact* contact) {
  b2Fixture* fixtureA = contact->GetFixtureA();
  if (!fixtureA->IsSensor()) {
    return (Tank*)( fixtureA->GetBody()->GetUserData() );
  }
  b2Fixture* fixtureB = contact->GetFixtureB();
  if (!fixtureB->IsSensor()) {
    return (Tank*)( fixtureB->GetBody()->GetUserData() );
  }
  return NULL;
}
void CollisionProcessor::BeginContact(b2Contact* contact) {
  StrategicPoint* point = GetStrategicPoint(contact);
  if (point == NULL) {
    return;
  }
  Tank* tank = GetTank(contact);
  if (tank == NULL) {
    return;
  }
  point->SetOwner(tank);
}

void CollisionProcessor::EndContact(b2Contact* contact) {
  StrategicPoint* point = GetStrategicPoint(contact);
  if (point == NULL) {
    return;
  }
  point->SetOwner(NULL);
}

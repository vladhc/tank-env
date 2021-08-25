#include "box2d/box2d.h"
#include "tank.h"
#include "collision_processor.h"
#include "strategic_point.h"
#include "game_object.h"

CollisionProcessor::CollisionProcessor(b2World* world) {
  world->SetContactListener(this);
}

CollisionProcessor::~CollisionProcessor() {
}

void CollisionProcessor::Step() {
}

struct TypedContact {
  Tank* tank;
  StrategicPoint* point;
};

TypedContact ToTypedContact(b2Contact* contact) {
  TypedContact c;

  GameObject* gameObjA = (GameObject*)contact->GetFixtureA()->GetBody()->GetUserData();
  switch (gameObjA->type) {
    case TANK:
      c.tank = (Tank*)gameObjA;
      break;
    case STRATEGIC_POINT:
      c.point = (StrategicPoint*)gameObjA;
      break;
  }

  GameObject* gameObjB = (GameObject*)contact->GetFixtureB()->GetBody()->GetUserData();
  switch (gameObjB->type) {
    case TANK:
      c.tank = (Tank*)gameObjB;
      break;
    case STRATEGIC_POINT:
      c.point = (StrategicPoint*)gameObjB;
      break;
  }

  return c;
}

void CollisionProcessor::BeginContact(b2Contact* contact) {
  TypedContact c = ToTypedContact(contact);
  if (c.tank == NULL || c.point == NULL) {
    return;
  }

  c.point->SetOwner(c.tank);
}

void CollisionProcessor::EndContact(b2Contact* contact) {
  TypedContact c = ToTypedContact(contact);
  if (c.point == NULL || c.tank == NULL) {
    return;
  }
  c.point->SetOwner(NULL);
}

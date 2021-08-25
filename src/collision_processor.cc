#include <iostream>
#include "box2d/box2d.h"
#include "tank.h"
#include "collision_processor.h"
#include "strategic_point.h"
#include "game_object.h"
#include "bullet.h"

CollisionProcessor::CollisionProcessor(b2World* world) {
  world->SetContactListener(this);
  createIterator = true;
}

CollisionProcessor::~CollisionProcessor() {
}

bool CollisionProcessor::PollEvent(TypedContact* c) {
  if (createIterator) {
    iter = contacts.begin();
    createIterator = false;
  }
  if (iter >= contacts.end()) {
    contacts.clear();
    createIterator = true;
    return false;
  }
  c = &(*iter);
  iter++;
  return true;
}

TypedContact ToTypedContact(b2Contact* contact) {
  TypedContact c{};

  b2Fixture* fixtures[] = {contact->GetFixtureA(), contact->GetFixtureB()};
  for (b2Fixture* fixture : fixtures) {
    GameObject* gameObj = (GameObject*)fixture->GetBody()->GetUserData();
    if (gameObj == NULL) {
      continue;
    }
    switch (gameObj->type) {
      case TANK:
        c.tank = (Tank*)gameObj;
        break;
      case STRATEGIC_POINT:
        c.point = (StrategicPoint*)gameObj;
        break;
      case BULLET:
        c.bullet = (Bullet*)gameObj;
        break;
    }
  }

  return c;
}

void CollisionProcessor::BeginContact(b2Contact* contact) {
  TypedContact c = ToTypedContact(contact);
  if (c.tank == NULL) {
    return;
  }
  if (c.point != NULL || c.bullet != NULL) {
    contacts.push_back(c);
    return;
  }
}

void CollisionProcessor::EndContact(b2Contact* contact) {
  TypedContact c = ToTypedContact(contact);
  if (c.point == NULL || c.tank == NULL) {
    return;
  }
  c.point->SetOwner(NULL);
}

#include <iostream>
#include "box2d/box2d.h"
#include "tank.h"
#include "collision_processor.h"
#include "strategic_point.h"
#include "game_object.h"
#include "bullet.h"

CollisionProcessor::CollisionProcessor(b2World* world) {
  world->SetContactListener(this);
  world->SetContactFilter(this);
  createIterator = true;
}

CollisionProcessor::~CollisionProcessor() {
}

bool CollisionProcessor::PollEvent(TypedContact* ptr) {
  if (createIterator) {
    iter = contacts.begin();
    createIterator = false;
  }
  if (iter >= contacts.end()) {
    contacts.clear();
    createIterator = true;
    return false;
  }
  *ptr = *iter;
  iter++;
  return true;
}

TypedContact ToTypedContact(b2Fixture *fixtureA, b2Fixture *fixtureB) {
  TypedContact c{NULL, NULL, NULL};

  b2Fixture* fixtures[] = {fixtureA, fixtureB};
  for (b2Fixture* fixture : fixtures) {
    GameObject* gameObj = (GameObject*)fixture->GetBody()->GetUserData();
    if (gameObj == NULL) {
      continue;
    }
    switch (gameObj->type) {
      case GameObjectType::TANK:
        c.tank = (Tank*)gameObj;
        break;
      case GameObjectType::STRATEGIC_POINT:
        c.point = (StrategicPoint*)gameObj;
        break;
      case GameObjectType::BULLET:
        c.bullet = (Bullet*)gameObj;
        break;
    }
  }

  return c;
}

TypedContact ToTypedContact(b2Contact* contact) {
  return ToTypedContact(contact->GetFixtureA(), contact->GetFixtureB());
}

void CollisionProcessor::BeginContact(b2Contact* contact) {
  TypedContact c = ToTypedContact(contact);
  contacts.push_back(c);
}

void CollisionProcessor::EndContact(b2Contact* contact) {
  TypedContact c = ToTypedContact(contact);
  if (c.point == NULL || c.tank == NULL) {
    return;
  }
  c.point->SetOwner(NULL);
}

bool CollisionProcessor::ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) {
  auto filterA = fixtureA->GetFilterData();
  auto filterB = fixtureB->GetFilterData();
  if ((filterA.maskBits & filterB.categoryBits) == 0 || (filterA.categoryBits & filterB.maskBits) == 0) {
    return false;
  }
  TypedContact c = ToTypedContact(fixtureA, fixtureB);
  if (c.bullet != NULL && c.tank != NULL && c.bullet->GetOwner() == c.tank) {
    return false;
  }
  return true;
}

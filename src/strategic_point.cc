#include <iostream>
#include "box2d/box2d.h"
#include "strategic_point.h"
#include "tank.h"
#include "game_object.h"

StrategicPoint::StrategicPoint(b2World* world, b2Vec2 position) :
  GameObject{GameObjectType::STRATEGIC_POINT}
{
  b2BodyDef bodyDef;
  bodyDef.type = b2_staticBody;
  bodyDef.position = position;

  body = world->CreateBody(&bodyDef);

  body->SetUserData(this);

  b2CircleShape shape;
  shape.m_radius = 0.5f;

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &shape;
  fixtureDef.isSensor = true;

  body->CreateFixture(&fixtureDef);
}

StrategicPoint::~StrategicPoint() {
  body->GetWorld()->DestroyBody(body);
}

Tank* StrategicPoint::GetOwner() {
  return owner;
}

void StrategicPoint::SetOwner(Tank* tank) {
  owner = tank;
}

b2Vec2 StrategicPoint::GetPosition() const {
  return body->GetPosition();
}


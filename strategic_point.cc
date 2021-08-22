#include "box2d/box2d.h"
#include "strategic_point.h"
#include "tank.h"

StrategicPoint::StrategicPoint(b2World* world, b2Vec2 position) {
  b2BodyDef bodyDef;
  bodyDef.type = b2_staticBody;
  bodyDef.position = position;

  body = world->CreateBody(&bodyDef);

  b2CircleShape shape;
  shape.m_radius = 0.5f;

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &shape;

  body->CreateFixture(&fixtureDef);
}

StrategicPoint::~StrategicPoint() {
  body->GetWorld()->DestroyBody(body);
}

Tank* StrategicPoint::GetOwner() {
  return owner;
}

b2Vec2 StrategicPoint::GetPosition() {
  return body->GetPosition();
}


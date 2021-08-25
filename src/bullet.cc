#include <iostream>
#include "box2d/box2d.h"
#include "game_object.h"
#include "bullet.h"

Bullet::Bullet(b2World* world, b2Vec2 firePosition, b2Vec2 fireDirection, GameObject* owner) :
  owner(owner),
  GameObject(BULLET)
{
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.bullet = true;
  bodyDef.position = firePosition;

  body = world->CreateBody(&bodyDef);
  body->SetUserData(this);

  b2CircleShape shape;
  shape.m_radius = 0.1f;

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &shape;

  body->CreateFixture(&fixtureDef);
  body->SetLinearVelocity(fireDirection);
}

Bullet::~Bullet() {
  body->GetWorld()->DestroyBody(body);
  owner = NULL;
  body = NULL;
}

b2Body* Bullet::GetBody() {
  return body;
}

GameObject* Bullet::GetOwner() {
  return owner;
}

#include <iostream>
#include "box2d/box2d.h"
#include "game_object.h"
#include "bullet.h"

Bullet::Bullet(b2World* world, b2Vec2 firePosition, b2Vec2 fireDirection, GameObject* owner) :
  GameObject(BULLET),
  owner(owner)
{
  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.bullet = true;
  bodyDef.position = firePosition;

  body = world->CreateBody(&bodyDef);
  body->SetUserData(this);

  b2CircleShape shape;
  shape.m_radius = 0.025f;

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &shape;
  fixtureDef.density = 1.0f;
  fixtureDef.filter.categoryBits = 0x0002;
  fixtureDef.filter.maskBits = 0x0001;

  body->CreateFixture(&fixtureDef);
  body->ApplyLinearImpulseToCenter(fireDirection, true);
}

Bullet::~Bullet() {
  body->GetWorld()->DestroyBody(body);
  owner = NULL;
  body = NULL;
}

b2Vec2 Bullet::GetPosition() const {
  return body->GetPosition();
}

b2Vec2 Bullet::GetLinearVelocity() const {
  return body->GetLinearVelocity();
}

GameObject* Bullet::GetOwner() {
  return owner;
}

#include <functional>
#include "geom.h"
#include "chunk.h"

void writeTankProp(float* arr, TankChunk prop, float value) {
  arr[static_cast<unsigned int>(prop)] = value;
}

void writeTankChunk(const Tank* tank, const Tank* hero, float* arr) {
  auto write = std::bind(
      writeTankProp,
      arr,
      std::placeholders::_1,
      std::placeholders::_2);
  write(TankChunk::IS_ENEMY, tank->GetTeamId() != hero->GetTeamId());

  write(TankChunk::HITPOINTS, tank->GetHitpoints());
  write(TankChunk::IS_ALIVE, tank->IsAlive());
  write(TankChunk::FIRE_COOLDOWN, tank->GetFireCooldown());

  const b2Vec2 pos = tank->GetPosition();
  const b2Vec2 posRelativeToBody = hero->GetLocalPoint(pos);
  write(
      TankChunk::POSITION_DISTANCE,
      posRelativeToBody.Length());

  write(
      TankChunk::POSITION_ANGLE,
      normalizeAngle(getAngle(posRelativeToBody), true));

  const auto angle = tank->GetAngle();
  const b2Vec2 unit = b2Vec2{std::cos(angle), std::sin(angle)};
  const b2Vec2 bodyDirection = hero->GetLocalPoint(unit);
  write(
      TankChunk::BODY_ANGLE,
      normalizeAngle(getAngle(bodyDirection), true));

  write(
      TankChunk::TURRET_ANGLE_RELATIVE_TO_BODY,
      normalizeAngle(tank->GetTurretAngle() - tank->GetAngle(), true));

  const b2Vec2 v = tank->GetLinearVelocity() - hero->GetLinearVelocity();
  write(TankChunk::VELOCITY_LENGTH, v.Length());
  write(
      TankChunk::VELOCITY_ANGLE_RELATIVE_TO_BODY,
      normalizeAngle(getAngle(v) - hero->GetAngle(), true));

  const float bodyAngularVelocity = tank->GetAngularVelocity();
  write(
      TankChunk::BODY_ANGULAR_VELOCITY,
      bodyAngularVelocity);
  write(
      TankChunk::TURRET_ANGULAR_VELOCITY,
      tank->GetTurretAngularVelocity() - bodyAngularVelocity);

  const b2Vec2 posRelativeToTurret = hero->GetTurretLocalPoint(pos);
  write(
      TankChunk::AIM_ANGLE,
      normalizeAngle(getAngle(posRelativeToTurret), true));
}

void writeHeroProp(float* arr, HeroChunk prop, float value) {
  arr[static_cast<unsigned int>(prop)] = value;
}

void writeHeroChunk(const Tank* hero, float* arr) {
  auto write = std::bind(
      writeHeroProp,
      arr,
      std::placeholders::_1,
      std::placeholders::_2);
  write(HeroChunk::HITPOINTS, hero->GetHitpoints());
  write(HeroChunk::FIRE_COOLDOWN, hero->GetFireCooldown());
  write(HeroChunk::POSITION_X, hero->GetPosition().x);
  write(HeroChunk::POSITION_Y, hero->GetPosition().y);

  const float bodyAngle = normalizeAngle(hero->GetAngle());
  write(HeroChunk::BODY_ANGLE, bodyAngle);

  write(
      HeroChunk::TURRET_ANGLE_RELATIVE_TO_BODY, 
      normalizeAngle(hero->GetTurretAngle() - bodyAngle, true));

  const b2Vec2 v = hero->GetLinearVelocity();
  write(
      HeroChunk::VELOCITY_ANGLE_RELATIVE_TO_BODY,
      normalizeAngle(getAngle(v) - bodyAngle, true));
  write(
      HeroChunk::VELOCITY_LENGTH,
      v.Length());

  const float bodyAngularV = hero->GetAngularVelocity();
  write(
      HeroChunk::BODY_ANGULAR_VELOCITY,
      bodyAngularV);
  write(
      HeroChunk::TURRET_ANGULAR_VELOCITY_RELATIVE_TO_BODY,
      hero->GetTurretAngularVelocity() - bodyAngularV);
}

void writeBulletProp(float* arr, BulletChunk prop, float value) {
  arr[static_cast<unsigned int>(prop)] = value;
}

void writeBulletChunk(const Bullet* bullet, const Tank* hero, float* arr) {
  auto write = std::bind(
      writeBulletProp,
      arr,
      std::placeholders::_1,
      std::placeholders::_2);

  const b2Vec2 pos = bullet->GetPosition();
  const b2Vec2 posRelative = hero->GetLocalPoint(pos);

  write(BulletChunk::POSITION_X, posRelative.x);
  write(BulletChunk::POSITION_Y, posRelative.y);
  write(BulletChunk::POSITION_DISTANCE, posRelative.Length());

  write(
      BulletChunk::POSITION_ANGLE,
      normalizeAngle(getAngle(posRelative), true));

  const b2Vec2 vAbsolute = bullet->GetLinearVelocity() - hero->GetLinearVelocity();
  const b2Vec2 v = hero->GetLocalPoint(vAbsolute + hero->GetPosition());
  write(BulletChunk::VELOCITY_X, v.x);
  write(BulletChunk::VELOCITY_Y, v.y);
  write(
      BulletChunk::VELOCITY_ANGLE,
      normalizeAngle(getAngle(vAbsolute) - hero->GetAngle(), true));
}

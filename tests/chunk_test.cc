#include <iostream>
#include <gtest/gtest.h>
#include "box2d/box2d.h"
#include "tank.h"
#include "env.h"
#include "chunk.h"

const float ERROR = 0.001;

void assertEq(float* arr, TankChunk prop, float expected) {
  auto idx = static_cast<unsigned int>(prop);
  float actual = arr[idx];
  ASSERT_FLOAT_EQ(actual, expected);
}

TEST(ChunkTest, WriteHeroChunk) {
  // GIVEN
  auto world = new b2World(b2Vec2{});
  Tank hero = Tank(1, 0, world);
  const b2Vec2 pos{-5., -2.5};
  const float bodyAngle = M_PI / 2;
  const float turretAngle = M_PI;
  hero.SetTransform(pos, bodyAngle, turretAngle);
  hero.Fire();
  hero.Drive(1, -1, 1);
  const unsigned int damage = 5;
  hero.TakeDamage(damage);

  ASSERT_TRUE(hero.GetAngularVelocity() > 0.);
  ASSERT_TRUE(hero.GetLinearVelocity().Length() > 0.);

  float* arr = new float[HeroChunk::Size];

  // WHEN
  writeHeroChunk(&hero, arr);

  // THEN
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(HeroChunk::HITPOINTS)],
      MAX_HITPOINTS - damage);
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(HeroChunk::FIRE_COOLDOWN)],
      MAX_FIRE_COOLDOWN - 1); // -1 due to calling Drive() after Fire()
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(HeroChunk::POSITION_X)],
      pos.x);
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(HeroChunk::POSITION_Y)],
      pos.y);
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(HeroChunk::BODY_ANGLE)],
      bodyAngle);
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(HeroChunk::TURRET_ANGLE_RELATIVE_TO_BODY)],
      turretAngle - bodyAngle);
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(HeroChunk::VELOCITY_ANGLE_RELATIVE_TO_BODY)],
      0.);
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(HeroChunk::VELOCITY_LENGTH)],
      hero.GetLinearVelocity().Length());
  const float bodyAngularV = hero.GetAngularVelocity();
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(HeroChunk::BODY_ANGULAR_VELOCITY)],
      bodyAngularV);
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(HeroChunk::TURRET_ANGULAR_VELOCITY_RELATIVE_TO_BODY)],
      hero.GetTurretAngularVelocity() - bodyAngularV);
}

TEST(ChunkTest, WriteTankChunk) {
  // GIVEN
  auto world = new b2World(b2Vec2{});
  Tank hero = Tank(1, 0, world);
  hero.SetTransform(b2Vec2{-5., 0}, M_PI / 2, M_PI);
  hero.Drive(0, 0, 1);

  Tank tank = Tank(2, 1, world);
  tank.SetTransform(b2Vec2{-10., 0}, 0, -M_PI / 2);
  tank.TakeDamage(5);
  tank.Drive(1, -1, 1);

  ASSERT_TRUE(tank.GetAngularVelocity() > 0.);
  ASSERT_TRUE(tank.GetLinearVelocity().Length() > 0.);

  float* arr = new float[TankChunk::Size];

  // WHEN
  writeTankChunk(&tank, &hero, arr);

  // THEN
  assertEq(arr, TankChunk::IS_ENEMY, 1);
  assertEq(arr, TankChunk::HITPOINTS, tank.GetHitpoints());
  assertEq(arr, TankChunk::IS_ALIVE, 1);
  assertEq(arr, TankChunk::FIRE_COOLDOWN, tank.GetFireCooldown());
  assertEq(arr, TankChunk::POSITION_DISTANCE, 5);
  assertEq(arr, TankChunk::POSITION_ANGLE, M_PI / 2);
  assertEq(arr, TankChunk::BODY_ANGLE, -M_PI / 2);
  assertEq(arr, TankChunk::TURRET_ANGLE_RELATIVE_TO_BODY, -M_PI / 2); 

  auto const v1 = hero.GetLinearVelocity().Length();
  auto const v2 = tank.GetLinearVelocity().Length();
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(TankChunk::VELOCITY_LENGTH)],
      sqrt(v1 * v1 + v2 * v2)
  );

  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(TankChunk::VELOCITY_ANGLE_RELATIVE_TO_BODY)],
      - (M_PI/2 + M_PI/4)
  );

  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(TankChunk::BODY_ANGULAR_VELOCITY)],
      tank.GetAngularVelocity()
  );

  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(TankChunk::TURRET_ANGULAR_VELOCITY)],
      tank.GetTurretAngularVelocity() - tank.GetAngularVelocity()
  );

  ASSERT_NEAR(
      arr[static_cast<unsigned int>(TankChunk::AIM_ANGLE)],
      0,
      0.00001
  );

}

TEST(ChunkTest, WriteTankChunkBodyAngle) {
  // GIVEN
  auto world = new b2World(b2Vec2{});
  Tank hero = Tank(1, 0, world);
  hero.SetTransform(b2Vec2{-5., -5.}, M_PI / 4, M_PI);

  Tank tank = Tank(2, 1, world);
  tank.SetTransform(b2Vec2{5., 0.}, -M_PI / 4, -M_PI / 4);

  float* arr = new float[TankChunk::Size];

  // WHEN
  writeTankChunk(&tank, &hero, arr);

  // THEN
  ASSERT_NEAR(
      arr[static_cast<unsigned int>(TankChunk::BODY_ANGLE)],
      -M_PI / 2,
      ERROR
  );
}

TEST(ChunkTest, WriteBulletChunk) {
  // GIVEN
  auto world = new b2World(b2Vec2{});
  Tank hero = Tank(1, 0, world);
  hero.SetTransform(b2Vec2{-5., 0}, M_PI / 2);

  for (int step=0; step < 300; step++) {
    hero.Drive(1, 0, 0);
    world->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
  }
  hero.SetTransform(b2Vec2{-5., 0}, M_PI / 2);

  Bullet bullet{world, b2Vec2{-5., -3.}, b2Vec2{1, 0}, &hero};
  float* arr = new float[BulletChunk::Size];

  // WHEN
  writeBulletChunk(&bullet, &hero, arr);

  // THEN
  ASSERT_NEAR(
      arr[static_cast<unsigned int>(BulletChunk::POSITION_X)],
      -3.,
      ERROR
  );
  ASSERT_NEAR(
      arr[static_cast<unsigned int>(BulletChunk::POSITION_Y)],
      0.,
      ERROR
  );
  ASSERT_EQ(
      arr[static_cast<unsigned int>(BulletChunk::POSITION_DISTANCE)],
      3.
  );
  ASSERT_NEAR(
      arr[static_cast<unsigned int>(BulletChunk::POSITION_ANGLE)],
      M_PI,
      ERROR
  );

  ASSERT_NEAR(
      arr[static_cast<unsigned int>(BulletChunk::VELOCITY_ANGLE)],
      -M_PI/2,
      ERROR
  );
}

TEST(ChunkTest, WriteObstacleChunk) {
  // GIVEN
  auto world = new b2World(b2Vec2{});
  Tank hero = Tank(1, 0, world);
  b2Vec2 heroPos{-5., 10};
  hero.SetTransform(heroPos, M_PI / 4);

  b2BodyDef bodyDef;
  bodyDef.type = b2_staticBody;
  auto obstacle = world->CreateBody(&bodyDef);

  const float width = 10;
  const float height = 2;
  b2PolygonShape obstacleShape;
  // args: half-width, half-height
  obstacleShape.SetAsBox(width / 2, height / 2);
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &obstacleShape;
  fixtureDef.density = 1.0f;
  obstacle->CreateFixture(&fixtureDef);

  b2Vec2 obstaclePos{5., 0};
  obstacle->SetTransform(obstaclePos, -M_PI / 4);

  float* arr = new float[ObstacleChunk::Size];

  // WHEN
  writeObstacleChunk(obstacle, &hero, arr);

  // THEN
  b2Vec2 v = obstaclePos - heroPos;
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(ObstacleChunk::POSITION_DISTANCE)],
      sqrt(v.x * v.x + v.y * v.y)
  );

  ASSERT_NEAR(
      arr[static_cast<unsigned int>(ObstacleChunk::ANGLE)],
      -M_PI / 2,
      ERROR
  );
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(ObstacleChunk::WIDTH)],
      width
  );
  ASSERT_FLOAT_EQ(
      arr[static_cast<unsigned int>(ObstacleChunk::HEIGHT)],
      height
  );
}

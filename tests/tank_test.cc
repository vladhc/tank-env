#include <gtest/gtest.h>
#include "tank.h"
#include "point.h"
#include <math.h>

TEST(TankTest, DamagedMoreThanHitpoints) {
  // GIVEN
  Tank tank = Tank(1, 1, new b2World(b2Vec2{}));
  auto hp = tank.GetHitpoints();

  // WHEN
  tank.TakeDamage(hp * 100);

  // THEN
  ASSERT_EQ(tank.GetHitpoints(), 0);
  ASSERT_FALSE(tank.IsAlive());
}

TEST(TankTest, TakeSmallAmountOfDamage) {
  // GIVEN
  Tank tank = Tank(1, 1, new b2World(b2Vec2{}));
  auto hp = tank.GetHitpoints();
  unsigned int damage = 1;

  // WHEN
  tank.TakeDamage(damage);

  // THEN
  ASSERT_TRUE(tank.GetHitpoints() == hp - damage);
}

TEST(TankTest, SetTransform) {
  // GIVEN
  const auto world = new b2World(b2Vec2{});
  Tank tank = Tank(0, 0, world);
  const b2Vec2 pos = b2Vec2{-10., 0};
  const float bodyAngle = -M_PI / 2;
  const float turretAngle = -(M_PI / 2 + M_PI / 4);

  // WHEN
  tank.SetTransform(pos, bodyAngle, turretAngle);

  // THEN
  EXPECT_TRUE(tank.GetPosition() == pos);
  EXPECT_TRUE(tank.GetAngle() == bodyAngle);
  EXPECT_TRUE(tank.GetTurretAngle() == turretAngle);
}

TEST(TankTest, GetTurretLocalPoint) {
  // GIVEN
  // Global X axis points right
  // Global Y axis points down
  const auto world = new b2World(b2Vec2{});
  Tank tank = Tank(0, 0, world);
  tank.SetTransform(b2Vec2{-10., 0}, -M_PI / 4, -3 * M_PI / 4);
  Tank enemy = Tank(1, 1, world);
  enemy.SetTransform(b2Vec2{-10., -5.}, M_PI);

  // WHEN
  const b2Vec2 pt = tank.GetTurretLocalPoint(enemy.GetPosition());

  // THEN
  EXPECT_TRUE(pt.x > 0);
  EXPECT_TRUE(pt.y > 0);
}

TEST(TankTest, GetLocalPoint) {
  // GIVEN
  const auto world = new b2World(b2Vec2{});
  Tank tank = Tank(0, 0, world);
  tank.SetTransform(b2Vec2{-10., 0}, -M_PI / 4);
  Tank enemy = Tank(1, 1, world);
  enemy.SetTransform(b2Vec2{-10., -5.}, M_PI);

  // WHEN
  const b2Vec2 pt = tank.GetLocalPoint(enemy.GetPosition());

  // THEN
  EXPECT_TRUE(pt.x > 0);
  EXPECT_TRUE(pt.y < 0);
}

TEST(TankTest, DeadTankHasMaxFireCooldown) {
  // GIVEN
  const auto world = new b2World(b2Vec2{});
  Tank tank = Tank(0, 0, world);

  // WHEN
  tank.TakeDamage(tank.GetHitpoints());

  // THEN
  EXPECT_EQ(tank.GetFireCooldown(), MAX_FIRE_COOLDOWN);
}

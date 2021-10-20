#include <gtest/gtest.h>
#include "tank.h"
#include "point.h"

TEST(TankTest, DamagedMoreThanHitpoints) {
  // GIVEN
  Tank tank = Tank(1, 1, new b2World(b2Vec2{}));
  auto hp = tank.GetHitpoints();

  // WHEN
  tank.TakeDamage(hp * 2);

  // THEN
  ASSERT_TRUE(tank.GetHitpoints() == 0);
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

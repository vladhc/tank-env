#include <gtest/gtest.h>
#include "tank.h"
#include "box2d/box2d.h"
#include "game_object.h"


TEST(LidarTest, RayContainsGameObject) {
  // GIVEN
  auto world = new b2World(b2Vec2{});

  Tank tank = Tank(0, 0, world);
  tank.SetTransform(b2Vec2{-5., 0}, 0);

  auto enemy = new Tank(1, 1, world);
  enemy->SetTransform(b2Vec2{0, 0}, 0);

  const unsigned int raysCount = 4;
  const float rayLength = 100;
  auto lidar = Lidar(world, tank.GetBody(), rayLength, raysCount);

  // WHEN
  auto rays = lidar.CastRays();

  // THEN
  ASSERT_EQ(rays.size(), raysCount);
  ASSERT_EQ(rays[0].obj, enemy);
  ASSERT_EQ(rays[1].obj, nullptr);
  ASSERT_EQ(rays[2].obj, nullptr);
  ASSERT_EQ(rays[3].obj, nullptr);
}

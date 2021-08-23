#include <gtest/gtest.h>
#include "tank.h"
#include "env.h"

const double PI = 3.14159265;
const double ABS_ERROR = 0.0001;

TEST(EnvTest, MoveTank) {
  // GIVEN
  Tank tank(
    Point{0.0, 0.0}, // position
    0 // angle
  );
  Point pt = Point{5., 0};
  tank.MoveTo(pt);
  int ticks = 200;

  // WHEN
  for (int i=0; i < ticks; i++) {
    moveTank(&tank);
  }

  // THEN
  EXPECT_NEAR(tank.GetPosition().x, pt.x, 0.1);
  EXPECT_NEAR(tank.GetPosition().y, pt.y, 0.1);
  EXPECT_FALSE(tank.GetMoveTarget().is_active);
}

TEST(EnvTest, RotateTankPI) {
  // GIVEN
  Tank tank(
    Point{0.0, 0.0}, // position
    0 // angle
  );
  tank.MoveTo(Point{-5., 0});
  int ticks = 200;

  // WHEN
  for (int i=0; i < ticks; i++) {
    rotateTank(&tank);
  }

  // THEN
  EXPECT_NEAR(tank.GetAngle(), PI, ABS_ERROR);
}

TEST(EnvTest, TankReachesMoveTarget) {
  // GIVEN
  Tank tank(
    Point{0.0, 0.0}, // position
    0 // angle
  );
  Point targetPos = Point{-3., 2.};
  tank.MoveTo(targetPos);
  int ticks = 200;

  // WHEN
  for (int i=0; i < ticks; i++) {
    moveTank(&tank);
    rotateTank(&tank);
  }

  // THEN
  EXPECT_NEAR(tank.GetPosition().x, targetPos.x, 0.1);
  EXPECT_NEAR(tank.GetPosition().y, targetPos.y, 0.1);
  EXPECT_FALSE(tank.GetMoveTarget().is_active);
}

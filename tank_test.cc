#include <gtest/gtest.h>
#include "tank.h"
#include "point.h"

const double ABS_ERROR = 0.0001;

TEST(TankTest, Accelerate) {
  // GIVEN
  Tank tank(
    Point{0.0, 0.0}, // position
    0 // angle
  );
  tank.MoveTo(Point{5., 0});
  int ticks = 2;

  // WHEN
  for (int i=0; i < ticks; i++) {
    tank.MoveTick();
  }

  // THEN
  double speed = tank.GetSpeed();
  EXPECT_NEAR(speed, ticks * 0.1, ABS_ERROR);
}

TEST(TankTest, Stop) {
  // GIVEN
  Tank tank(
    Point{0.0, 0.0}, // position
    0 // angle
  );
  tank.MoveTo(Point{5., 0});
  tank.MoveTick();
  tank.MoveTick();

  // WHEN
  tank.Stop(true);

  // THEN
  EXPECT_NEAR(tank.GetSpeed(), 0., ABS_ERROR);
  EXPECT_FALSE(tank.GetMoveTarget().is_active);
}

TEST(TankTest, Rotate) {
  // GIVEN
  Tank tank(
    Point{0.0, 0.0}, // position
    0 // angle
  );

  // WHEN
  tank.Rotate(0.5);

  // THEN
  EXPECT_NEAR(tank.GetAngle(), 0.3, ABS_ERROR);
}

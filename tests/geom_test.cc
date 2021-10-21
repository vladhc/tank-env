#include <math.h>
#include <gtest/gtest.h>
#include "geom.h"
#include "box2d/box2d.h"

TEST(Geom, GetAngle45Degrees) {
  // GIVEN
  const b2Vec2 v{0.5, 0.5};

  // WHEN
  const float angle = getAngle(v);

  // THEN
  ASSERT_FLOAT_EQ(angle, M_PI / 4);
}

TEST(Geom, GetAngle90Degrees) {
  // GIVEN
  const b2Vec2 v{0.0, 5.0};

  // WHEN
  const float angle = getAngle(v);

  // THEN
  ASSERT_FLOAT_EQ(angle, M_PI / 2);
}

TEST(Geom, GetAngle0Degrees) {
  // GIVEN
  const b2Vec2 v{0.5, 0.0};

  // WHEN
  const float angle = getAngle(v);

  // THEN
  ASSERT_FLOAT_EQ(angle, 0);
}

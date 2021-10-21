#include <iostream>
#include <math.h>
#include "geom.h"
#include "box2d/box2d.h"

float normalizeAngle(float angle, bool plusMinusPi) {
  if (plusMinusPi) {
    while (angle > M_PI) {
      angle -= 2 * M_PI;
    }
    while (angle < -M_PI) {
      angle += 2 * M_PI;
    }
  } else {
    while (angle < 0) {
      angle += 2 * M_PI;
    }
    while (angle > 2 * M_PI) {
      angle -= 2 * M_PI;
    }
  }
  return angle;
}

float getAngle(const b2Vec2& pos) {
  return atan2(pos.y, pos.x);
}

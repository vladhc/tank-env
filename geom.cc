#include <iostream>
#include <math.h>
#include "geom.h"
#include "box2d/box2d.h"

#define PI 3.14159265

float calcDistance(b2Vec2 p1, b2Vec2 p2) {
  float dx = p2.x - p1.x;
  float dy = p2.y - p1.y;
  return sqrt(dx * dx + dy * dy);
}

float normalizeAngle(float angle, bool plusMinusPi) {
  if (plusMinusPi) {
    while (angle > PI) {
      angle -= 2 * PI;
    }
    while (angle < -PI) {
      angle += 2 * PI;
    }
  } else {
    while (angle < 0) {
      angle += 2 * PI;
    }
    while (angle > 2 * PI) {
      angle -= 2 * PI;
    }
  }
  return angle;
}

float angleDelta(b2Vec2 src, b2Vec2 target, float curAngle) {
  float dx = target.x - src.x;
  float dy = target.y - src.y;
  float targetAngle = atan2(dy, dx);
  targetAngle = normalizeAngle(targetAngle);
  curAngle = normalizeAngle(curAngle);

  float delta = targetAngle - curAngle;
  return normalizeAngle(delta, true);
}

float abs2(float x) {
  if (x >= 0) {
    return x;
  }
  return -x;
}

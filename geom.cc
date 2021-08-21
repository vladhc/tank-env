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

float angleDelta(b2Vec2 src, b2Vec2 target, float curAngle) {
  float dx = target.x - src.x;
  float dy = target.y - src.y;
  float targetAngle = atan2(dy, dx);
  while (targetAngle < 0) {
    targetAngle += 2 * PI;
  }
  while (targetAngle > 2 * PI) {
    targetAngle -= 2 * PI;
  }
  while (curAngle < 0) {
    curAngle += 2 * PI;
  }
  while (curAngle > 2.0) {
    curAngle -= 2 * PI;
  }
  float delta = targetAngle - curAngle;
  while (delta > PI) {
    delta -= 2 * PI;
  }
  while (delta < -PI) {
    delta += 2 * PI;
  }
  return delta;
}

float abs2(float x) {
  if (x >= 0) {
    return x;
  }
  return -x;
}

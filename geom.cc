#include <math.h>
#include "geom.h"
#include "point.h"

const double PI = 3.14159265;

double calcDistance(Point p1, Point p2) {
  double dx = p2.x - p1.x;
  double dy = p2.y - p1.y;
  return sqrt(dx * dx + dy * dy);
}

double angleDelta(Point src, Point target, double curAngle) {
  double dx = target.x - src.x;
  double dy = target.y - src.y;
  double targetAngle = atan2(dy, dx);
  if (targetAngle < 0) {
    targetAngle += 2 * PI;
  }
  if (curAngle < 0) {
    curAngle += 2 * PI;
  }
  return targetAngle - curAngle;
}

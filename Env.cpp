#include "Point.h"
#include "Tank.h"

int main() {
  Point start{0.0, 0.0};
  Tank tank(
        3, // max_speed
        0.3, // acceleration
        0.1, // angle_acceleration
        10.0, // fire_range
        20.0, // vision_range
        10, // max_fire_cooldown
        start, // position
        0.0, // angle
        100 // hit_points
  );
  tank.Tick();
}

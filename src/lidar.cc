#include "lidar.h"
#include <vector>

class RayCastCallback : public b2RayCastCallback{
  public:
    RayCastCallback(b2Vec2 pos);
    float ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float fraction);
    void PassFinished();
    std::vector<Ray> GetRays();
  private:
    std::vector<Ray> rays;
    b2Vec2 pos;
    GameObject* nearestObj;
    b2Vec2 nearestPt;
    float nearestIntersectionFraction;
};

const float MAX_RAY_LENGTH = 100000000;

RayCastCallback::RayCastCallback(b2Vec2 pos) :
  pos{pos},
  nearestObj{NULL},
  nearestIntersectionFraction{MAX_RAY_LENGTH}
{}

float RayCastCallback::ReportFixture(
    b2Fixture *fixture,
    const b2Vec2 &point,
    const b2Vec2 &normal,
    float fraction) {
  if (fraction < nearestIntersectionFraction) {
    nearestObj = (GameObject*)fixture->GetBody()->GetUserData();
    nearestPt = point - pos;
    nearestIntersectionFraction = fraction;
  }
  return 1;
}

void RayCastCallback::PassFinished() {
  rays.push_back(Ray{nearestPt, nearestObj});
  nearestObj = NULL;
  nearestIntersectionFraction = MAX_RAY_LENGTH;
}

std::vector<Ray> RayCastCallback::GetRays() {
  return rays;
}

Lidar::Lidar(const b2World* world, const b2Body* body, float rayLength, unsigned int raysCount) :
    world{world},
    body{body},
    rayLength{rayLength},
    raysCount{raysCount}
{}

std::vector<Ray> Lidar::CastRays() const {
  const float angleDelta = 2 * M_PI / raysCount;
  const b2Vec2 pos = body->GetPosition();
  RayCastCallback callback{pos};
  for (unsigned i=0; i < raysCount; i++) {
    float angle = angleDelta * i;
    b2Vec2 ray{rayLength * std::cos(angle), rayLength * std::sin(angle)};
    ray = body->GetWorldVector(ray);
    world->RayCast(&callback, pos, ray);
    callback.PassFinished();
  }
  return callback.GetRays();
}

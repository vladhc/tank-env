#pragma once
#include "box2d/box2d.h"
#include <vector>
#include "game_object.h"

struct Ray {
  const b2Vec2 pt;
  const GameObject* obj;
};

class Lidar {
  public:
    Lidar(const b2World* world, const b2Body* body, float rayLength, unsigned int raysCount);
    std::vector<Ray> CastRays() const;
  private:
    const b2World* world;
    const b2Body* body;
    float rayLength;
    unsigned int raysCount;
};

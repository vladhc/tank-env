#pragma once
#include "box2d/box2d.h"

float calcDistance(b2Vec2 p1, b2Vec2 p2);

float angleDelta(b2Vec2 src, b2Vec2 target, float curAngle);

float abs2(float x);

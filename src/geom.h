#pragma once
#include "box2d/box2d.h"

float normalizeAngle(float curAngle, bool plusMinusPi=false);

float getAngle(const b2Vec2& position);

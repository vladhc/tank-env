#pragma once
#include "tank.h"

enum struct TankChunk {
  IS_ENEMY,
  HITPOINTS,
  IS_ALIVE,
  FIRE_COOLDOWN,
  POSITION_DISTANCE,
  POSITION_ANGLE,
  BODY_ANGLE,
  TURRET_ANGLE_RELATIVE_TO_BODY,
  VELOCITY_LENGTH,
  VELOCITY_ANGLE_RELATIVE_TO_BODY,
  BODY_ANGULAR_VELOCITY,
  TURRET_ANGULAR_VELOCITY,
  AIM_ANGLE, // angle between hero's turret and the tank
  Size // Keep this member the last one
};

enum struct HeroChunk {
  HITPOINTS,
  FIRE_COOLDOWN,
  POSITION_X,
  POSITION_Y,
  BODY_ANGLE,
  TURRET_ANGLE_RELATIVE_TO_BODY,
  VELOCITY_ANGLE_RELATIVE_TO_BODY,
  VELOCITY_LENGTH,
  BODY_ANGULAR_VELOCITY,
  TURRET_ANGULAR_VELOCITY_RELATIVE_TO_BODY,
  Size // Keep this member the last one
};

enum struct BulletChunk {
  POSITION_X,
  POSITION_Y,
  POSITION_DISTANCE,
  POSITION_ANGLE,
  VELOCITY_X,
  VELOCITY_Y,
  VELOCITY_ANGLE,
  Size // Keep this member the last one
};

void writeTankChunk(const Tank* tank, const Tank* hero, float* arr);

void writeHeroChunk(const Tank* hero, float* arr);

void writeBulletChunk(const Bullet* bullet, const Tank* hero, float* arr);

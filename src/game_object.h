#pragma once

enum GameObjectType {
  TANK,
  BULLET,
  STRATEGIC_POINT
};

class GameObject {
  public:
    GameObject(GameObjectType gType);
    GameObjectType type;
};

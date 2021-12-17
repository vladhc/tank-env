#pragma once

enum struct GameObjectType {
  TANK,
  BULLET,
  STRATEGIC_POINT
};

class GameObject {
  public:
    GameObject(GameObjectType gType);
    GameObjectType type;
};

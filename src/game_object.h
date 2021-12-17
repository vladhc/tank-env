#pragma once

enum struct GameObjectType {
  TANK,
  BULLET
};

class GameObject {
  public:
    GameObject(GameObjectType gType);
    GameObjectType type;
};

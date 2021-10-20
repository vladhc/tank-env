#include "action.h"

class KeyboardController {
  public:
    KeyboardController();
    Action GetAction();
    bool IsExit() const;
  private:
    bool left;
    bool right;
    bool turretLeft;
    bool turretRight;
    bool up;
    bool down;
    bool fire;
    bool exit;
    void Update();
};

#include "action.h"

class KeyboardController {
  public:
    KeyboardController();
    Action GetAction();
    bool IsExit();
  private:
    bool left;
    bool right;
    bool up;
    bool down;
    bool fire;
    bool exit;
    void Update();
};

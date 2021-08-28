#include "env.h"
#include "renderer.h"
#include "keyboard_controller.h"
#include <SDL2/SDL.h>

int main() {
  Renderer r;

  Env env = Env();
  env.Reset();
  KeyboardController keyController;

  std::map<int, Action> actions;
  actions[0] = keyController.GetAction();

  while(true) {
    r.Render(env);
    env.Step(actions);
    actions[0] = keyController.GetAction();
    if (keyController.IsExit()) {
      break;
    }
    SDL_Delay(TIME_STEP * 1000);
  }

  delete &r;
}

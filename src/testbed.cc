#include <iostream>
#include <math.h>
#include "env.h"
#include "renderer.h"
#include "keyboard_controller.h"
#include <SDL2/SDL.h>
#include "geom.h"

int main() {
  Renderer r;

  Env env{10, 24};
  env.Reset();
  KeyboardController keyController;

  std::map<int, Action> actions;
  actions[0] = keyController.GetAction();

  auto tank = env.GetTanks()[0];
  while(true) {
    r.Render(env);
    env.Step(actions);
    actions[0] = keyController.GetAction();
    if (keyController.IsExit()) {
      break;
    }
    // auto pos = tank->GetPosition();
    // std::cout << "(" << pos.x << "," << pos.y << ") ";
    // std::cout << "bodyAngle: " << (tank->GetAngle() / M_PI) << "; ";
    // std::cout << "turretAngle: " << (tank->GetTurretAngle() / M_PI) << "; ";
    // std::cout << "angularVelocity: " << (tank->GetAngularVelocity()) << "; ";
    // std::cout << "turretAngularVelocity: " << (tank->GetTurretAngularVelocity() - tank->GetAngularVelocity()) << "; ";

    // b2Vec2 v = tank->GetLinearVelocity();

    // b2Vec2 vLocal = tank->GetLocalPoint(v);
    // std::cout << "v: " << v.Length() << "; ";
    // std::cout << std::endl;

    SDL_Delay(TIME_STEP * 1000);
  }

  delete &r;
}

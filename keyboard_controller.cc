#include "env.h"
#include "keyboard_controller.h"
#include <SDL2/SDL.h>

KeyboardController::KeyboardController():
  left(false),
  right(false),
  up(false),
  down(false),
  exit(false)
{
}

bool KeyboardController::IsExit() {
  return exit;
}

void KeyboardController::Update() {
  SDL_Event event;

  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_w:
          up = true;
          break;
        case SDLK_s:
          down = true;
          break;
        case SDLK_d:
          right = true;
          break;
        case SDLK_a:
          left = true;
          break;
        case SDLK_x:
          exit = true;
          break;
      }
    } else if (event.type == SDL_KEYUP) {
      switch (event.key.keysym.sym) {
        case SDLK_w:
          up = false;
          break;
        case SDLK_s:
          down = false;
          break;
        case SDLK_d:
          right = false;
          break;
        case SDLK_a:
          left = false;
          break;
        case SDLK_x:
          exit = false;
          break;
      }
    }
  }
}

Action KeyboardController::GetAction() {
  Update();
  Action action{0.0f, 0.0f};
  if (left) {
    action.anglePower = -1.0f;
  }
  if (right) {
    action.anglePower = 1.0f;
  }
  if (up) {
    action.power = 1.0f;
  }
  if (down) {
    action.power = -1.0f;
  }
  return action;
}

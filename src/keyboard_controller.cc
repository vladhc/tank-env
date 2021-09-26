#include "env.h"
#include "keyboard_controller.h"
#include <SDL2/SDL.h>

KeyboardController::KeyboardController():
  left(false),
  right(false),
  turretLeft(false),
  turretRight(false),
  up(false),
  down(false),
  fire(false),
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
        case SDLK_q:
          turretLeft = true;
          break;
        case SDLK_e:
          turretRight = true;
          break;
        case SDLK_d:
          right = true;
          break;
        case SDLK_a:
          left = true;
          break;
        case SDLK_SPACE:
          fire = true;
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
        case SDLK_q:
          turretLeft = false;
          break;
        case SDLK_e:
          turretRight = false;
          break;
        case SDLK_d:
          right = false;
          break;
        case SDLK_a:
          left = false;
          break;
        case SDLK_SPACE:
          fire = false;
          break;
      }
    }
    if (event.type == SDL_QUIT) {
      exit = true;
    }
  }
}

Action KeyboardController::GetAction() {
  Update();
  Action action{0.0f, 0.0f, 0.0f, fire};
  if (left) {
    action.anglePower = -1.0f;
  }
  if (right) {
    action.anglePower = 1.0f;
  }
  if (turretLeft) {
    action.turretAnglePower = -1.0f;
  }
  if (turretRight) {
    action.turretAnglePower = 1.0f;
  }
  if (up) {
    action.power = 1.0f;
  }
  if (down) {
    action.power = -1.0f;
  }
  return action;
}

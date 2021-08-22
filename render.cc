#include <iostream>
#include <SDL2/SDL.h>
#include <stdio.h>
#include "env.h"
#include "point.h"
#include "keyboard_controller.h"

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const double SCALE = 5;

const int TANK_BODY_POINTS_COUNT = 5;
const Point TANK_BODY_POINTS[TANK_BODY_POINTS_COUNT] = {
  Point{-1, -0.6},
  Point{1, -0.6},
  Point{1, 0.6},
  Point{-1, 0.6},
  Point{-1, -0.6}
};
const int TANK_TURRET_POINTS_COUNT = 9;
const Point TANK_TURRET_POINTS[TANK_TURRET_POINTS_COUNT] = {
  Point{-0.6, -0.5},
  Point{0.5, -0.5},
  Point{0.5, -0.2},
  Point{1.0, -0.2},
  Point{1.0, 0.2},
  Point{0.5, 0.2},
  Point{0.5, 0.5},
  Point{-0.6, 0.5},
  Point{-0.6, -0.5}
};
const double TANK_LAYER_OFFSET = 0.1;

void drawArena(float size, SDL_Renderer* gRenderer) {
  const Point border[5] = {
    Point{-size, -size},
    Point{-size, size},
    Point{size, size},
    Point{size, -size},
    Point{-size, -size}
  };
  SDL_Point points[5];
  for (int i=0; i < 5; i++) {
    Point pt = border[i];
    points[i] = SDL_Point{pt.x * SCALE, pt.y * SCALE};
  }
  SDL_SetRenderDrawColor(gRenderer, 0x73, 0x6E, 0x74, 0xFF);
  SDL_RenderDrawLines(gRenderer, points, TANK_BODY_POINTS_COUNT);
}

void drawTank(Tank* tank, SDL_Renderer* gRenderer) {
  b2Vec2 pos = tank->GetPosition();
  double angle = tank->GetAngle();
  double size = tank->GetSize();

  // Move target
  SDL_SetRenderDrawColor(gRenderer, 0x72, 0x72, 0x18, 0x80);
  Target target = tank->GetMoveTarget();
  if (target.is_active) {
    SDL_RenderDrawLine(
      gRenderer,
      pos.x * SCALE,
      pos.y * SCALE,
      target.coord.x * SCALE,
      target.coord.y * SCALE
    );
  }

  SDL_Point points[TANK_BODY_POINTS_COUNT];

  // Dark lines (body bottom)
  for (int i=0; i < TANK_BODY_POINTS_COUNT; i++) {
    Point pt = TANK_BODY_POINTS[i];
    pt = Point{
      pt.x * size,
      pt.y * size
    };
    points[i] = SDL_Point{
      (pos.x + pt.x * cos(angle) - pt.y * sin(angle)) * SCALE,
      (pos.y + pt.y * cos(angle) + pt.x * sin(angle)) * SCALE
    };
  }
  SDL_SetRenderDrawColor(gRenderer, 0x73, 0x6E, 0x74, 0xFF);
  SDL_RenderDrawLines(gRenderer, points, TANK_BODY_POINTS_COUNT);

  // Light lines (body top)
  for (int i=0; i < TANK_BODY_POINTS_COUNT; i++) {
    points[i].y = points[i].y - int(TANK_LAYER_OFFSET * SCALE);
  }
  SDL_SetRenderDrawColor(gRenderer, 0xEF, 0xEC, 0xE7, 0xFF);
  SDL_RenderDrawLines(gRenderer, points, TANK_BODY_POINTS_COUNT);

  // Light lines (turret top)
  SDL_Point turretPoints[TANK_TURRET_POINTS_COUNT];
  for (int i=0; i < TANK_TURRET_POINTS_COUNT; i++) {
    Point pt = Point{
      TANK_TURRET_POINTS[i].x * size,
      TANK_TURRET_POINTS[i].y * size
    };
    turretPoints[i] = SDL_Point{
      (pos.x + pt.x * cos(angle) - pt.y * sin(angle)) * SCALE,
      (pos.y + pt.y * cos(angle) + pt.x * sin(angle)) * SCALE
    };
    turretPoints[i].y = turretPoints[i].y - int(TANK_LAYER_OFFSET * SCALE);
  }
  SDL_SetRenderDrawColor(gRenderer, 0x73, 0x6E, 0x74, 0xFF);
  SDL_RenderDrawLines(gRenderer, turretPoints, TANK_TURRET_POINTS_COUNT);

  for (int i=0; i < TANK_TURRET_POINTS_COUNT; i++) {
    turretPoints[i].y = turretPoints[i].y - int(TANK_LAYER_OFFSET * SCALE);
  }
  SDL_SetRenderDrawColor(gRenderer, 0xEF, 0xEC, 0xE7, 0xFF);
  SDL_RenderDrawLines(gRenderer, turretPoints, TANK_TURRET_POINTS_COUNT);
}


int main() {
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  //Create window
  SDL_Window* window = SDL_CreateWindow(
      "Tank Environment",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH, SCREEN_HEIGHT,
      SDL_WINDOW_SHOWN);
  if(window == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Renderer* gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderClear(gRenderer);

  //Update screen
  SDL_RenderPresent(gRenderer);

  Env env = Env();
  Observation obs = env.Reset();
  KeyboardController keyController;
  Action action = keyController.GetAction();

  while(true) {
    auto t = env.Step(action);
    Observation obs = std::get<0>(t);
    Tank* tank = obs.tank;

    // Render observation
    SDL_SetRenderDrawColor(gRenderer, 0xBC, 0xB6, 0x54, 0xFF );
    SDL_RenderClear(gRenderer);
    drawArena(obs.arenaSize, gRenderer);
    drawTank(tank, gRenderer);
    SDL_RenderPresent(gRenderer);

    // Evaluate next action
    action = keyController.GetAction();
    if (keyController.IsExit()) {
      break;
    }
    SDL_Delay(TIME_STEP * 1000);
  }

  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
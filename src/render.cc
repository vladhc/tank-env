#include <iostream>
#include <SDL2/SDL.h>
#include <stdio.h>
#include "env.h"
#include "point.h"
#include "strategic_point.h"
#include "bullet.h"
#include "keyboard_controller.h"

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const float SCALE = 2.5f;
const int OFFSET_X = SCREEN_WIDTH / 2;
const int OFFSET_Y = SCREEN_HEIGHT / 2;

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
    points[i] = SDL_Point{
      pt.x * SCALE + OFFSET_X,
      pt.y * SCALE + OFFSET_Y
    };
  }
  SDL_SetRenderDrawColor(gRenderer, 0x73, 0x6E, 0x74, 0xFF);
  SDL_RenderDrawLines(gRenderer, points, TANK_BODY_POINTS_COUNT);
}

void drawStrategicPoint(StrategicPoint* point, SDL_Renderer* gRenderer) {
  b2Vec2 pos = point->GetPosition();
  SDL_SetRenderDrawColor(gRenderer, 0x72, 0x72, 0x18, 0x80);
  SDL_Rect rect{
    pos.x * SCALE - 2 + OFFSET_X,
    pos.y * SCALE - 2 + OFFSET_Y,
    4,
    4
  };

  SDL_RenderFillRect(gRenderer, &rect);
}

void drawBullet(Bullet* bullet, SDL_Renderer* gRenderer) {
  b2Vec2 pos = bullet->GetBody()->GetPosition();
  SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x72, 0x18, 0xFF);
  SDL_Rect rect{
    pos.x * SCALE - 5 + OFFSET_X,
    pos.y * SCALE - 5 + OFFSET_Y,
    10,
    10
  };
  SDL_RenderFillRect(gRenderer, &rect);
}

void drawTank(Tank* tank, SDL_Renderer* gRenderer) {
  b2Vec2 pos = tank->GetPosition();
  double angle = tank->GetAngle();
  double size = tank->GetSize();

  SDL_Point points[TANK_BODY_POINTS_COUNT];

  // Dark lines (body bottom)
  for (int i=0; i < TANK_BODY_POINTS_COUNT; i++) {
    Point pt = TANK_BODY_POINTS[i];
    pt = Point{
      pt.x * size,
      pt.y * size
    };
    points[i] = SDL_Point{
      (pos.x + pt.x * cos(angle) - pt.y * sin(angle)) * SCALE + OFFSET_X,
      (pos.y + pt.y * cos(angle) + pt.x * sin(angle)) * SCALE + OFFSET_Y
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
      (pos.x + pt.x * cos(angle) - pt.y * sin(angle)) * SCALE + OFFSET_X,
      (pos.y + pt.y * cos(angle) + pt.x * sin(angle)) * SCALE + OFFSET_Y
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

  SDL_Renderer* gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  SDL_RenderClear(gRenderer);

  //Update screen
  SDL_RenderPresent(gRenderer);

  Env env = Env();
  env.Reset();
  KeyboardController keyController;

  Action actions[env.GetTanks().size()];
  for (int i=0; i < env.GetTanks().size(); i++) {
    actions[i] = Action{0.0f, 0.0f};
  }
  actions[0] = keyController.GetAction();

  while(true) {
    // Render environment
    SDL_SetRenderDrawColor(gRenderer, 0xBC, 0xB6, 0x54, 0xFF );
    SDL_RenderClear(gRenderer);
    drawArena(env.GetArenaSize(), gRenderer);

    std::vector<Tank*> tanks = env.GetTanks();
    for (Tank* tank : tanks) {
      drawTank(tank, gRenderer);
    }
    for (Bullet* bullet : env.GetBullets()) {
      drawBullet(bullet, gRenderer);
    }

    drawStrategicPoint(env.GetStrategicPoint(), gRenderer);
    SDL_RenderPresent(gRenderer);

    // Evaluate next action
    auto t = env.Step(actions);
    actions[0] = keyController.GetAction();
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

#include <iostream>
#include <exception>
#include <SDL2/SDL.h>
#include <stdio.h>
#include "env.h"
#include "point.h"
#include "strategic_point.h"
#include "bullet.h"
#include "renderer.h"

//Screen dimension constants
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;

const float SCALE = 15.0f;
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
const float TANK_LAYER_OFFSET = 0.1f;

Renderer::Renderer() {
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    throw std::runtime_error("SDL could not initialize");
  }

  //Create window
  window = SDL_CreateWindow(
      "Tank Environment",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH, SCREEN_HEIGHT,
      SDL_WINDOW_SHOWN);
  if(window == NULL) {
    throw std::runtime_error("Window could not be created");
  }

  gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  SDL_RenderClear(gRenderer);
  SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);

  //Update screen
  SDL_RenderPresent(gRenderer);
}

Renderer::~Renderer() {
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Renderer::DrawArena(float size) {
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
      int(pt.x * SCALE + OFFSET_X),
      int(pt.y * SCALE + OFFSET_Y)
    };
  }
  SDL_SetRenderDrawColor(gRenderer, 0x73, 0x6E, 0x74, 0xFF);
  SDL_RenderDrawLines(gRenderer, points, TANK_BODY_POINTS_COUNT);
}

void Renderer::DrawStrategicPoint(const StrategicPoint& point) {
  b2Vec2 pos = point.GetPosition();
  SDL_SetRenderDrawColor(gRenderer, 0x72, 0x72, 0x18, 0x80);
  SDL_Rect rect{
    int(pos.x * SCALE - 2 + OFFSET_X),
    int(pos.y * SCALE - 2 + OFFSET_Y),
    4,
    4
  };

  SDL_RenderFillRect(gRenderer, &rect);
}

void Renderer::DrawBullet(const Bullet& bullet) {
  b2Vec2 pos = bullet.GetPosition();
  SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x72, 0x18, 0xFF);
  SDL_Rect rect{
    int(pos.x * SCALE - 5 + OFFSET_X),
    int(pos.y * SCALE - 5 + OFFSET_Y),
    10,
    10
  };
  SDL_RenderFillRect(gRenderer, &rect);
}

void Renderer::DrawTank(const Tank& tank) {
  const b2Vec2 pos = tank.GetPosition();
  const float angle = tank.GetAngle();
  const float size = tank.GetSize();

  SDL_Point points[TANK_BODY_POINTS_COUNT];

  // Dark lines (body bottom)
  for (int i=0; i < TANK_BODY_POINTS_COUNT; i++) {
    Point pt = TANK_BODY_POINTS[i];
    pt = Point{
      pt.x * size,
      pt.y * size
    };
    points[i] = SDL_Point{
      int((pos.x + pt.x * cos(angle) - pt.y * sin(angle)) * SCALE + OFFSET_X),
      int((pos.y + pt.y * cos(angle) + pt.x * sin(angle)) * SCALE + OFFSET_Y)
    };
  }
  uint8 alpha = 0xFF;
  if (!tank.IsAlive()) {
    alpha = 0x88;
  }
  SDL_SetRenderDrawColor(gRenderer, 0xEF, 0xEC, 0xE7, alpha);
  SDL_RenderDrawLines(gRenderer, points, TANK_BODY_POINTS_COUNT);

  // Light lines (turret top)
  const float turretAngle = tank.GetTurretAngle();
  SDL_Point turretPoints[TANK_TURRET_POINTS_COUNT];
  for (int i=0; i < TANK_TURRET_POINTS_COUNT; i++) {
    Point pt = Point{
      TANK_TURRET_POINTS[i].x * size,
      TANK_TURRET_POINTS[i].y * size
    };
    turretPoints[i] = SDL_Point{
      int((pos.x + pt.x * cos(turretAngle) - pt.y * sin(turretAngle)) * SCALE + OFFSET_X),
      int((pos.y + pt.y * cos(turretAngle) + pt.x * sin(turretAngle)) * SCALE + OFFSET_Y)
    };
    turretPoints[i].y = turretPoints[i].y - int(TANK_LAYER_OFFSET * SCALE);
  }
  SDL_SetRenderDrawColor(gRenderer, 0xEF, 0xEC, 0xE7, alpha);
  SDL_RenderDrawLines(gRenderer, turretPoints, TANK_TURRET_POINTS_COUNT);
}

void Renderer::Render(const Env &env) {
  SDL_SetRenderDrawColor(gRenderer, 0xBC, 0xB6, 0x54, 0xFF );
  SDL_RenderClear(gRenderer);
  DrawArena(env.GetArenaSize());

  for (const Tank* tank : env.GetTanks()) {
    DrawTank(*tank);
  }
  for (const Bullet* bullet : env.GetBullets()) {
    DrawBullet(*bullet);
  }

  DrawStrategicPoint(*env.GetStrategicPoint());
  SDL_RenderPresent(gRenderer);
}

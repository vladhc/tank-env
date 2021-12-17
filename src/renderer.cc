#include <iostream>
#include <exception>
#include <SDL2/SDL.h>
#include <stdio.h>
#include "env.h"
#include "lidar.h"
#include "point.h"
#include "bullet.h"
#include "renderer.h"

//Screen dimension constants
const int SCREEN_WIDTH = 2 * 800;
const int SCREEN_HEIGHT = 2 * 800;

const float SCALE = 2 * 15.0f;
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
  uint8 alpha = 0xFF;
  if (!tank.IsAlive()) {
    alpha = 0x88;
  }
  if (tank.GetTeamId() == 0) {
    SDL_SetRenderDrawColor(gRenderer, 0x71, 0x71, 0xEC, alpha);
  } else {
    SDL_SetRenderDrawColor(gRenderer, 0x11, 0xEC, 0x11, alpha);
  }
  DrawBody(*tank.GetBody());
  DrawBody(*tank.GetTurret());
}

void Renderer::DrawLidar(const Tank& tank) {
  SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0x77);
  auto lidar = tank.GetLidar();
  std::vector<Ray> rays = lidar->CastRays();
  auto pos = tank.GetPosition();
  for (const Ray ray : rays) {
    SDL_RenderDrawLine(
        gRenderer,
        pos.x * SCALE + OFFSET_X,
        pos.y * SCALE + OFFSET_Y,
        (pos.x + ray.pt.x) * SCALE + OFFSET_X,
        (pos.y + ray.pt.y) * SCALE + OFFSET_Y);
  }
}

void Renderer::DrawObstacle(const b2Body& obstacle) {
  SDL_SetRenderDrawColor(gRenderer, 0x73, 0x6E, 0x74, 0xFF);
  DrawBody(obstacle);
}

void Renderer::DrawBody(const b2Body& body) {
  const b2Fixture* fixture = body.GetFixtureList();
  while (fixture != NULL) {
    const b2Shape* shape = fixture->GetShape();
    if (shape->m_type == 0) { // Circle
      const b2CircleShape* circle = static_cast<const b2CircleShape*>(shape);
      size_t pointsCount = 32;
      const float angleDelta = 2 * M_PI / pointsCount;
      SDL_Point points[pointsCount + 1];
      for (unsigned int i=0; i < pointsCount; i++) {
        float angle = angleDelta * i;
        b2Vec2 pt{
          circle->m_p.x + circle->m_radius * std::cos(angle),
          circle->m_p.y + circle->m_radius * std::sin(angle)
        };
        pt = body.GetWorldPoint(pt);
        points[i] = SDL_Point{
          int(pt.x * SCALE + OFFSET_X),
          int(pt.y * SCALE + OFFSET_Y)
        };
      }
      points[pointsCount] = points[0];
      SDL_RenderDrawLines(gRenderer, points, pointsCount + 1);
    }
    else if (shape->m_type == 2) {
      const b2PolygonShape* poly = static_cast<const b2PolygonShape*>(shape);
      SDL_Point points[poly->m_count + 1];
      for (int i=0; i < poly->m_count; i++) {
        auto pt = body.GetWorldPoint(poly->m_vertices[i]);
        points[i] = SDL_Point{
          int(pt.x * SCALE + OFFSET_X),
          int(pt.y * SCALE + OFFSET_Y)
        };
      }
      points[poly->m_count] = points[0];
      SDL_RenderDrawLines(gRenderer, points, poly->m_count + 1);
    }
    fixture = fixture->GetNext();
  }
}

void Renderer::Render(const Env &env) {
  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF );
  SDL_RenderClear(gRenderer);
  DrawArena(env.GetArenaSize());

  for (const Tank* tank : env.GetTanks()) {
    if (tank->GetId() == 0) {
      DrawLidar(*tank);
    }
    DrawTank(*tank);
  }
  for (const Bullet* bullet : env.GetBullets()) {
    DrawBullet(*bullet);
  }
  for (const auto obstacle : env.GetObstacles()) {
    DrawObstacle(*obstacle);
  }

  SDL_RenderPresent(gRenderer);
}

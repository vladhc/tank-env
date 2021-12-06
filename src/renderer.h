#include <SDL2/SDL.h>
#include "box2d/box2d.h"
#include "tank.h"
#include "bullet.h"

class Renderer {
  public:
    Renderer();
    ~Renderer();
    void Render(const Env &env);
  private:
    SDL_Window* window;
    SDL_Renderer* gRenderer;
    void DrawArena(float size);
    // void DrawStrategicPoint(const StrategicPoint& point);
    void DrawBullet(const Bullet& bullet);
    void DrawTank(const Tank& tank);
    void DrawLidar(const Tank& tank);
    void DrawObstacle(const b2Body& obstacle);
    void DrawBody(const b2Body& shape);
};

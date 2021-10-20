#include <SDL2/SDL.h>

class Renderer {
  public:
    Renderer();
    ~Renderer();
    void Render(const Env &env);
  private:
    SDL_Window* window;
    SDL_Renderer* gRenderer;
    void DrawArena(float size);
    void DrawStrategicPoint(const StrategicPoint& point);
    void DrawBullet(const Bullet& bullet);
    void DrawTank(const Tank& tank);
};

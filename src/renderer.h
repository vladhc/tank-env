#include <SDL2/SDL.h>

class Renderer {
  public:
    Renderer();
    ~Renderer();
    void Render(Env &env);
  private:
    SDL_Window* window;
    SDL_Renderer* gRenderer;
    void DrawArena(float size);
    void DrawStrategicPoint(StrategicPoint* point);
    void DrawBullet(Bullet* bullet);
    void DrawTank(Tank* tank);
};

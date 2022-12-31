#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class RenderWindow
{
public:
    RenderWindow(const char* title, int width, int height);
    SDL_Texture* loadTexture(const char* filePath);
    void CleanUp();
    void clear();
    void render(SDL_Texture* texture, SDL_Rect& src, SDL_Rect& dst);
    void render(SDL_Texture* texture, SDL_Rect& dst);
    void display();

    int getRefreshRate();

private:
    SDL_Window* window_;
    SDL_Renderer* renderer_;
};
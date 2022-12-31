#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "render_window.hpp"

RenderWindow::RenderWindow(const char* title, int width, int height)
    : window_{NULL}, renderer_{NULL}
{
    window_ = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window_ == NULL)
        std::cout << "Window failed to init. Error: " << SDL_GetError() << "\n";

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (renderer_ == NULL)
        std::cout << "Renderer failed to init. Error: " << SDL_GetError() << "\n";
}

SDL_Texture* RenderWindow::loadTexture(const char* filePath)
{
    SDL_Texture* texture = NULL;
    texture = IMG_LoadTexture(renderer_, filePath);

    if (texture == NULL)
        std::cout << "Failed to load texture. Error: " << SDL_GetError() << "\n";

    return texture;
}

void RenderWindow::CleanUp()
{
    SDL_DestroyWindow(window_);
}

void RenderWindow::clear()
{
    SDL_SetRenderDrawColor(renderer_, 0, 0, 255, 255);
    SDL_RenderClear(renderer_);
}

void RenderWindow::render(SDL_Texture* texture, SDL_Rect& src, SDL_Rect& dst)
{
    SDL_RenderCopy(renderer_, texture, &src, &dst); // the two nulls are the viewable portion of the texture. This means it is all viewable
}

void RenderWindow::render(SDL_Texture* texture, SDL_Rect& dst)
{
    SDL_RenderCopy(renderer_, texture, NULL, &dst); // the two nulls are the viewable portion of the texture. This means it is all viewable
}

void RenderWindow::display()
{
    SDL_RenderPresent(renderer_);
}

int RenderWindow::getRefreshRate()
{
    int display_index = SDL_GetWindowDisplayIndex(window_);
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(display_index, 0, &mode);
    return mode.refresh_rate;
}
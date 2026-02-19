#include "common_ui/window.hpp"
#include <SDL.h>

namespace aff::sdl_utils::common {

Window::Window(const std::string& title, int w, int h, Uint32 flags)
{
    SDL_Window* win = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
    if (!win) throw Error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    window_ = unique_window(win);

    SDL_Renderer* ren = SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) throw Error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());
    renderer_ = unique_renderer(ren);
}

Window::~Window()
{
    // unique handles will clean up
}

void Window::clear(const SDL_Color& color)
{
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer_.get());
}

void Window::present()
{
    SDL_RenderPresent(renderer_.get());
}

} // namespace aff::sdl_utils::common

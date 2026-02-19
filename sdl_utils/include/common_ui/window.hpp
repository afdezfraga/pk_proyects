#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_WINDOW_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_WINDOW_HPP


#include "sdl_handle.hpp"
#include "sdl_error.hpp"
#include <SDL2/SDL.h>

#include <string>

namespace aff::sdl_utils::common {

class Window {
public:
    Window(const std::string& title, int w, int h, Uint32 flags = SDL_WINDOW_SHOWN);
    ~Window();

    SDL_Window* raw() const noexcept { return window_.get(); }
    SDL_Renderer* renderer() const noexcept { return renderer_.get(); }

    void clear(const SDL_Color& color);
    void present();

    Window(Window&&) noexcept = default;
    Window& operator=(Window&&) noexcept = default;
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

private:
    unique_window window_;
    unique_renderer renderer_;
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_WINDOW_HPP
#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_SDL_HANDLE_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_SDL_HANDLE_HPP


#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include <memory>

namespace aff::sdl_utils::common {

template<typename T, void(*Deleter)(T*)>
struct unique_sdl_handle {
    using pointer = T*;

    explicit unique_sdl_handle(pointer p = nullptr) noexcept : ptr(p) {}
    ~unique_sdl_handle() noexcept { reset(); }

    unique_sdl_handle(unique_sdl_handle&& o) noexcept : ptr(o.ptr) { o.ptr = nullptr; }
    unique_sdl_handle& operator=(unique_sdl_handle&& o) noexcept { if (this!=&o) { reset(); ptr=o.ptr; o.ptr=nullptr;} return *this; }

    unique_sdl_handle(const unique_sdl_handle&) = delete;
    unique_sdl_handle& operator=(const unique_sdl_handle&) = delete;

    pointer get() const noexcept { return ptr; }
    pointer release() noexcept { pointer p = ptr; ptr = nullptr; return p; }
    void reset(pointer p = nullptr) noexcept { if (ptr) Deleter(ptr); ptr = p; }
    explicit operator bool() const noexcept { return ptr != nullptr; }

private:
    pointer ptr = nullptr;
};

using unique_window = unique_sdl_handle<SDL_Window, SDL_DestroyWindow>;
using unique_renderer = unique_sdl_handle<SDL_Renderer, SDL_DestroyRenderer>;
using unique_texture = unique_sdl_handle<SDL_Texture, SDL_DestroyTexture>;
using unique_surface = unique_sdl_handle<SDL_Surface, SDL_FreeSurface>;
using unique_font = unique_sdl_handle<TTF_Font, TTF_CloseFont>;

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_SDL_HANDLE_HPP
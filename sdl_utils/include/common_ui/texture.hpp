#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_TEXTURE_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_TEXTURE_HPP


#include "sdl_handle.hpp"
#include <SDL2/SDL.h>

#include <utility>

namespace aff::sdl_utils::common {

class Texture {
public:
    explicit Texture(SDL_Texture* t = nullptr) noexcept : tex_(t) {}
    Texture(Texture&&) noexcept = default;
    Texture& operator=(Texture&&) noexcept = default;
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    SDL_Texture* raw() const noexcept { return tex_.get(); }

    int width() const noexcept;
    int height() const noexcept;

private:
    unique_texture tex_;
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_TEXTURE_HPP
#include "common_ui/texture.hpp"
#include <SDL.h>

namespace aff::sdl_utils::common {

int Texture::width() const noexcept
{
    if (!tex_.get()) return 0;
    int w = 0; SDL_QueryTexture(tex_.get(), nullptr, nullptr, &w, nullptr);
    return w;
}

int Texture::height() const noexcept
{
    if (!tex_.get()) return 0;
    int h = 0; SDL_QueryTexture(tex_.get(), nullptr, nullptr, nullptr, &h);
    return h;
}

} // namespace aff::sdl_utils::common

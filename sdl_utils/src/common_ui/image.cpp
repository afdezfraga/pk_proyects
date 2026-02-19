#include "common_ui/image.hpp"
#include "common_ui/sdl_error.hpp"
#include <SDL_image.h>

namespace aff::sdl_utils::common {

Texture Image::load(const std::string& path, SDL_Renderer* renderer)
{
    SDL_Surface* surf = IMG_Load(path.c_str());
    if (!surf) throw Error(std::string("IMG_Load failed: ") + IMG_GetError());

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!tex) throw Error(std::string("SDL_CreateTextureFromSurface failed: ") + SDL_GetError());

    return Texture(tex);
}

} // namespace aff::sdl_utils::common

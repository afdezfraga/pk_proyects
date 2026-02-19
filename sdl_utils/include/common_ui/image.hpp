#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_IMAGE_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_IMAGE_HPP


#include "texture.hpp"
#include <SDL2/SDL.h>

#include <string>

namespace aff::sdl_utils::common {

class Image {
public:
    // Load image and create texture using provided renderer. Throws on error.
    static Texture load(const std::string& path, SDL_Renderer* renderer);
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_IMAGE_HPP
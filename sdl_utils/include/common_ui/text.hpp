#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_TEXT_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_TEXT_HPP


#include "font.hpp"
#include "texture.hpp"
#include <SDL2/SDL.h>

#include <string>

namespace aff::sdl_utils::common {

class Text {
public:
    // Render text to a texture using blended rendering.
    static Texture render(const Font& font, const std::string& text, SDL_Color color, SDL_Renderer* renderer);
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_TEXT_HPP
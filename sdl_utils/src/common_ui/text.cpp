#include "common_ui/text.hpp"
#include "common_ui/sdl_error.hpp"
#include <SDL_ttf.h>
#include <SDL.h>

namespace aff::sdl_utils::common {

Texture Text::render(const Font& font, const std::string& text, SDL_Color color, SDL_Renderer* renderer)
{
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font.raw(), text.c_str(), color);
    if (!surf) throw Error(std::string("TTF_RenderUTF8_Blended failed: ") + TTF_GetError());

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!tex) throw Error(std::string("SDL_CreateTextureFromSurface failed: ") + SDL_GetError());

    return Texture(tex);
}

} // namespace aff::sdl_utils::common

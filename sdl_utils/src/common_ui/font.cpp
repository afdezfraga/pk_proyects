#include "common_ui/font.hpp"
#include <SDL_ttf.h>

namespace aff::sdl_utils::common {

Font::Font(const std::string& path, int ptsize)
    : ptsize_(ptsize)
{
    TTF_Font* f = TTF_OpenFont(path.c_str(), ptsize);
    if (!f) throw Error(std::string("TTF_OpenFont failed: ") + TTF_GetError());
    font_ = unique_font(f);
}

Font::~Font() = default;

} // namespace aff::sdl_utils::common

#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_FONT_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_FONT_HPP


#include "sdl_handle.hpp"
#include "sdl_error.hpp"
#include <SDL_ttf.h>

#include <string>

namespace aff::sdl_utils::common {

class Font {
public:
    Font(const std::string& path, int ptsize);
    ~Font();

    TTF_Font* raw() const noexcept { return font_.get(); }
    int ptsize() const noexcept { return ptsize_; }

    Font(Font&&) noexcept = default;
    Font& operator=(Font&&) noexcept = default;
    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

private:
    unique_font font_;
    int ptsize_ = 0;
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_FONT_HPP
#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_THEME_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_THEME_HPP

#include <SDL.h>
#include <string>
#include <optional>
#include <unordered_map>

namespace aff::sdl_utils::common {

class Theme {
public:
    Theme() = default;
    ~Theme() = default;


    SDL_Color primary{0,170,255,255};
    SDL_Color background{30,30,30,255};
    SDL_Color accent{255,80,80,255};
    float scale = 1.0f;

    std::optional<SDL_Color> get(const std::string& name) const {
        if (name == "primary") return primary;
        if (name == "background") return background;
        if (name == "accent") return accent;
        auto it = extra_colors_.find(name);
        if (it != extra_colors_.end()) return it->second;
        return std::nullopt;
    }

private:
    std::unordered_map<std::string, SDL_Color> extra_colors_ {}; // for caching rendered text textures
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_THEME_HPP
#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_SDL_ERROR_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_SDL_ERROR_HPP


#include <stdexcept>
#include <string>

namespace aff::sdl_utils::common {

class Error : public std::runtime_error {
public:
    explicit Error(const std::string& msg) : std::runtime_error(msg) {}
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_SDL_ERROR_HPP
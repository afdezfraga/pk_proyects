#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_SDL_MANAGER_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_SDL_MANAGER_HPP


#include "sdl_error.hpp"
#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <cstdint>
#include <atomic>
#include <string>

namespace aff::sdl_utils::common {

class SDLManager {
public:
    explicit SDLManager(uint32_t sdl_flags = SDL_INIT_VIDEO, uint32_t img_flags = IMG_INIT_PNG | IMG_INIT_JPG);
    ~SDLManager();

    bool initialized() const noexcept { return initialized_; }

    // Convenience singleton access; creates a static manager with default flags.
    static SDLManager& instance();

    SDLManager(const SDLManager&) = delete;
    SDLManager& operator=(const SDLManager&) = delete;

private:
    std::atomic<bool> initialized_ = false;
    uint32_t sdl_flags_ = 0;
    uint32_t img_flags_ = 0;
};

} // namespace aff::sdl_utils::common

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_SDL_MANAGER_HPP
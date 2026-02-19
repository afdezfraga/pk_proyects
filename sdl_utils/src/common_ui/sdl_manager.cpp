#include "common_ui/sdl_manager.hpp"

#include <sstream>

namespace aff::sdl_utils::common {

SDLManager::SDLManager(uint32_t sdl_flags, uint32_t img_flags)
    : initialized_(false), sdl_flags_(sdl_flags), img_flags_(img_flags)
{
    if (SDL_Init(sdl_flags_) != 0) {
        std::ostringstream ss;
        ss << "SDL_Init failed: " << SDL_GetError();
        throw Error(ss.str());
    }

    int imgFlags = img_flags_;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        std::ostringstream ss;
        ss << "IMG_Init failed: " << IMG_GetError();
        SDL_Quit();
        throw Error(ss.str());
    }

    if (TTF_Init() != 0) {
        std::ostringstream ss;
        ss << "TTF_Init failed: " << TTF_GetError();
        IMG_Quit();
        SDL_Quit();
        throw Error(ss.str());
    }

    initialized_ = true;
}

SDLManager::~SDLManager()
{
    if (!initialized_) return;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    initialized_ = false;
}

SDLManager& SDLManager::instance()
{
    static SDLManager mgr(SDL_INIT_VIDEO, IMG_INIT_PNG | IMG_INIT_JPG);
    return mgr;
}

} // namespace aff::sdl_utils::common

#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_LAYOUT_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_LAYOUT_HPP

#include <SDL2/SDL.h>
#include <string>

namespace aff::sdl_utils::common {

    class layout {
    public:
        virtual ~layout() = default;

        // Initialize the layout with the renderer
        virtual void initialize(SDL_Renderer* renderer) = 0;

        // Render the layout
        virtual void render(SDL_Renderer* renderer) = 0;

        // Handle events
        virtual void handleEvent(const SDL_Event& event) = 0;
    };

}

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_LAYOUT_HPP
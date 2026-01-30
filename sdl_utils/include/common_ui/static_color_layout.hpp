#ifndef AFF_PK_PROJECTS_SDL_UTILS_COMMON_STATIC_COLOR_LAYOUT_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_COMMON_STATIC_COLOR_LAYOUT_HPP

#include <SDL2/SDL.h>
#include <string>

#include <common_ui/layout.hpp>

namespace aff::sdl_utils::common {

    class static_color_layout : public layout {
    public:
        explicit static_color_layout(const SDL_Color& color) : bg_color(color) {}

        void initialize(SDL_Renderer* /*renderer*/) override {
            // No initialization needed for static color layout
        }

        void render(SDL_Renderer* renderer) override {
            // Set the draw color to the background color
            SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
            // Fill the entire renderer with the background color
            SDL_RenderClear(renderer);
        }

        void handleEvent(const SDL_Event& /*event*/) override {
            // No event handling needed for static color layout
        }

    private:
        SDL_Color bg_color;

    };

}

#endif // AFF_PK_PROJECTS_SDL_UTILS_COMMON_STATIC_COLOR_LAYOUT_HPP
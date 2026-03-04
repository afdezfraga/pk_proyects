#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SDL_CONFIG_BG_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SDL_CONFIG_BG_HPP

#include <SDL2/SDL.h>

namespace aff::pk_high_low::views {

class sdl_config_bg {

private:
    Uint32 anim_period_ {4000};
    int line_spacing_ {64};
    SDL_Color bg_color_ { 211, 47, 47, 255 };
    SDL_Color white_color_ { 255, 255, 255, 255 };
    SDL_Color blue_color_ { 3, 169, 244, 255 };
    const Uint8 opacity_40_ { 102 }; // 40% opacity for anti-aliasing

public:
    // Rule of 0

    // Animated red background with diagonal black lines
    void render(SDL_Renderer* renderer)
    {
        Uint32 ticks = SDL_GetTicks();
        
        // Fill background with red
        SDL_SetRenderDrawColor(renderer, bg_color_.r, bg_color_.g, bg_color_.b, bg_color_.a);
        SDL_RenderClear(renderer);

        // Diagonal lines
        int w, h; SDL_GetRendererOutputSize(renderer, &w, &h);
        float anim = (ticks % anim_period_) / static_cast<float>(anim_period_); // 0..1

        int limit = std::max(w, h); // enough to cover the whole screen diagonally
        limit += line_spacing_ - (limit % line_spacing_); // round up to a multiple of line_spacing
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int i = -limit; i < limit; i += line_spacing_) {
            int x1 = i;
            int y1 = 0;
            int x2 = i + h;
            int y2 = h;
            SDL_SetRenderDrawColor(renderer, blue_color_.r, blue_color_.g, blue_color_.b, opacity_40_);
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2); // left-top to right-bottom
            SDL_RenderDrawLine(renderer, x2, y1, x1, y2); // right-top to left-bottom
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

};

} // namespace aff::pk_high_low::views

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SDL_CONFIG_BG_HPP

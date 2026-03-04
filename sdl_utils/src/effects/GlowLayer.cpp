#include "effects/GlowLayer.hpp"
#include <algorithm>

namespace aff::sdl_utils::effects {

// naive filled circle approximation with alpha gradient
void GlowLayer::renderGlow(SDL_Renderer* renderer, int cx, int cy, int r, SDL_Color color) {
    // clamp
    if (r <= 0) return;

    for (int dy = -r; dy <= r; ++dy) {
        int y = cy + dy;
        int dx = static_cast<int>(std::sqrt((double)r * r - dy * dy));
        int x0 = cx - dx;
        int x1 = cx + dx;
        SDL_BlendMode prev_mode;
        SDL_GetRenderDrawBlendMode(renderer, &prev_mode); // save current blend mode
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        // compute alpha falloff based on distance from center
        for (int x = x0; x <= x1; ++x) {
            int dist = static_cast<int>(std::sqrt((x-cx)*(x-cx) + (y-cy)*(y-cy)));
            float t = 1.0f - (float)dist / (float)r;
            if (t <= 0.0f) continue;
            Uint8 a = static_cast<Uint8>(color.a * t);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, a);
            SDL_RenderDrawPoint(renderer, x, y);
        }
        SDL_SetRenderDrawBlendMode(renderer, prev_mode); // restore previous blend mode
    }
}

} // namespace aff::sdl_utils::effects

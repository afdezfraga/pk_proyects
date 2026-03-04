#ifndef AFF_PK_PROJECTS_SDL_UTILS_EFFECTS_GLOWLAYER_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_EFFECTS_GLOWLAYER_HPP

#include <SDL.h>

namespace aff::sdl_utils::effects {

class GlowLayer {
public:
    GlowLayer() = default;
    ~GlowLayer() = default;

    // Render a simple radial glow (approx) at center (cx,cy) with radius r and color
    static void renderGlow(SDL_Renderer* renderer, int cx, int cy, int r, SDL_Color color);
};

} // namespace aff::sdl_utils::effects

#endif // AFF_PK_PROJECTS_SDL_UTILS_EFFECTS_GLOWLAYER_HPP
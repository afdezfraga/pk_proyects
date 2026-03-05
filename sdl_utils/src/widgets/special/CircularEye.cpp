#include "widgets/special/CircularEye.hpp"
#include <SDL.h>
#include <cmath>

namespace aff::sdl_utils::widgets::special {

static void drawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int r, SDL_Color color) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int dy = -r; dy <= r; ++dy) {
        int y = cy + dy;
        int dx = static_cast<int>(std::sqrt((double)r * r - dy * dy));
        int x0 = cx - dx;
        int x1 = cx + dx;
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLine(renderer, x0, y, x1, y);
    }
}

// draw a circular orbit as a series of small dots; a thicker rotating segment
// is produced by drawing larger dots over a sub-arc defined by centerAngle/segAngle
// draw a thin dotted orbit
static void drawOrbitThin(SDL_Renderer* renderer, int cx, int cy, int r, SDL_Color color,
                          int dotRadius = 1) {
    const float step = M_PI / 90.0f; // ~2 degrees
    for (float a = 0.0f; a < 2.0f * M_PI; a += step) {
        float dx = std::cos(a);
        float dy = std::sin(a);
        int px = static_cast<int>(cx + dx * r + 0.5f);
        int py = static_cast<int>(cy + dy * r + 0.5f);
        drawFilledCircle(renderer, px, py, dotRadius, color);
    }
}

// draw a continuous arc segment (handles wrap-around)
static void drawArcSegment(SDL_Renderer* renderer, int cx, int cy, int r, SDL_Color color,
                           int thickness, float startAngle, float endAngle) {
    const float step = M_PI / 720.0f; // fine grain for smoothness (~0.25 deg)
    // normalize to [0,2pi)
    const float TWO_PI = 2.0f * M_PI;
    while (startAngle < 0) startAngle += TWO_PI;
    while (endAngle < 0) endAngle += TWO_PI;
    while (startAngle >= TWO_PI) startAngle -= TWO_PI;
    while (endAngle >= TWO_PI) endAngle -= TWO_PI;

    auto draw_range = [&](float a0, float a1) {
        for (float a = a0; a <= a1 + 1e-6f; a += step) {
            float dx = std::cos(a);
            float dy = std::sin(a);
            int px = static_cast<int>(cx + dx * r + 0.5f);
            int py = static_cast<int>(cy + dy * r + 0.5f);
            drawFilledCircle(renderer, px, py, thickness, color);
        }
    };

    if (endAngle >= startAngle) {
        draw_range(startAngle, endAngle);
    } else {
        // wraps around 2pi: draw start->2pi and 0->end
        draw_range(startAngle, TWO_PI);
        draw_range(0.0f, endAngle);
    }
}

CircularEye::CircularEye() = default;

void CircularEye::update(float dt) {
    // rotate faster
    rotation_ += (dt / 2000.0f) * M_PI; // half rotation every 2 seconds
    if (rotation_ > 2.0f * M_PI) rotation_ -= 2.0f * M_PI;
    // small breathing on focus
    focus_ = 0.5f + 0.5f * std::sin(SDL_GetTicks() / 1000.0f);
}

void CircularEye::render(SDL_Renderer* renderer) {
    if (!visible()) return;
    int cx = x() + w() / 2;
    int cy = y() + h() / 2;
    int outer_r = std::min(w(), h()) / 2;

    // iris (outer ring)
    SDL_Color iris = iris_color_;
    drawFilledCircle(renderer, cx, cy, outer_r, iris);

    // orbits: multiple concentric rings around the eye
    SDL_Color orbitCol{0,170,255,120};
    // inner thin orbit
    drawOrbitThin(renderer, cx, cy, outer_r + 10, orbitCol, 1);
    // middle faint orbit
    SDL_Color orbitCol2{0,170,255,90};
    drawOrbitThin(renderer, cx, cy, outer_r + 22, orbitCol2, 1);
    // outer orbit with rotating thicker segment
    SDL_Color orbitCol3{0,170,255,200};
    float segLen = M_PI / 6.0f; // 30 degrees
    drawOrbitThin(renderer, cx, cy, outer_r + 34, orbitCol3, 1);
    float half = segLen * 0.5f;
    float startA = rotation_ - half;
    float endA = rotation_ + half;
    drawArcSegment(renderer, cx, cy, outer_r + 34, orbitCol3, 3, startA, endA);

    // inner pupil
    int pupil_r = static_cast<int>(outer_r * 0.65f);
    drawFilledCircle(renderer, cx, cy, pupil_r, pupil_color_);

    // glow
    aff::sdl_utils::effects::GlowLayer::renderGlow(renderer, cx, cy, outer_r + 12, glow_color_);

    // specular highlight
    SDL_Color highlight{255,255,255,200};
    int hx = cx - pupil_r/4;
    int hy = cy - pupil_r/4;
    drawFilledCircle(renderer, hx, hy, pupil_r/4, highlight);

    // white glow
    // aff::sdl_utils::effects::GlowLayer::renderGlow(renderer, hx, hy, (pupil_r/4) + 6, SDL_Color{255,255,255,100});
}

} // namespace aff::sdl_utils::widgets::special

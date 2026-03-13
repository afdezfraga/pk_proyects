#ifndef AFF_PK_PROJECTS_SDL_UTILS_WIDGETS_SPECIAL_CIRCULAREYE_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_WIDGETS_SPECIAL_CIRCULAREYE_HPP

#include <common_ui/Widget.hpp>
#include <effects/GlowLayer.hpp>
#include <SDL.h>

namespace aff::sdl_utils::widgets::special {

class CircularEye : public aff::sdl_utils::common::Widget {
public:
    CircularEye();
    ~CircularEye() override = default;

    void setRotation(float r) { rotation_ = r; }
    void setFocus(float f) { focus_ = f; }
    void setPupilColor(SDL_Color c) { pupil_color_ = c; }

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    void render_animated(SDL_Renderer* renderer);

private:
    float rotation_ = 0.0f; // radians
    float focus_ = 0.0f; // 0..1
    SDL_Color iris_color_{0,170,255,255};
    SDL_Color pupil_color_{0,0,0,255};
    SDL_Color glow_color_{0,170,255,128};
};

} // namespace aff::sdl_utils::widgets::special

#endif // AFF_PK_PROJECTS_SDL_UTILS_WIDGETS_SPECIAL_CIRCULAREYE_HPP

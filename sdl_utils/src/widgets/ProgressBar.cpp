#include "widgets/ProgressBar.hpp"
#include <SDL.h>

namespace aff::sdl_utils::widgets {

ProgressBar::ProgressBar() = default;

void ProgressBar::setValue(float v) {
    if (v < min_) v = min_;
    if (v > max_) v = max_;
    value_ = (v - min_) / (max_ - min_);
}

void ProgressBar::setRange(float minv, float maxv) {
    min_ = minv; max_ = maxv;
}

void ProgressBar::update(float dt) {
    // simple linear interpolation toward target
    const float speed = 4.0f; // smoothing speed
    displayValue_ += (value_ - displayValue_) * std::min(1.0f, speed * dt);
}

void ProgressBar::render(SDL_Renderer* renderer) {
    if (!visible()) return;
    SDL_Rect r{ x(), y(), w(), h() };
    // background
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &r);

    // fill
    SDL_Rect fill = r;
    fill.w = static_cast<int>(r.w * displayValue_);
    SDL_SetRenderDrawColor(renderer, 0, 170, 255, 255);
    SDL_RenderFillRect(renderer, &fill);

    // border
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderDrawRect(renderer, &r);
}

} // namespace aff::sdl_utils::widgets

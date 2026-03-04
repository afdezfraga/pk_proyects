#ifndef AFF_PK_PROJECTS_SDL_UTILS_WIDGETS_PROGRESSBAR_HPP
#define AFF_PK_PROJECTS_SDL_UTILS_WIDGETS_PROGRESSBAR_HPP

#include <common_ui/Widget.hpp>
#include <common_ui/Theme.hpp>

namespace aff::sdl_utils::widgets {

class ProgressBar : public aff::sdl_utils::common::Widget {
public:
    ProgressBar();
    ~ProgressBar() override = default;

    void setValue(float v); // 0..1
    void setRange(float minv, float maxv);

    void render(SDL_Renderer* renderer) override;
    void update(float dt) override;

private:
    float value_ = 0.0f;
    float displayValue_ = 0.0f; // smoothed
    float min_ = 0.0f, max_ = 1.0f;
    aff::sdl_utils::common::Theme theme_;
};

} // namespace aff::sdl_utils::widgets

#endif // AFF_PK_PROJECTS_SDL_UTILS_WIDGETS_PROGRESSBAR_HPP
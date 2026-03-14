#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SYSTEMCHECK_VIEW_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SYSTEMCHECK_VIEW_HPP

#include <SDL2/SDL.h>
#include <controller/settings_api.hpp>
#include <common_ui/window.hpp>
#include <views/sdl_config_bg.hpp>

#include <widgets/ProgressBar.hpp>
#include <widgets/Button.hpp>
#include <widgets/special/CircularEye.hpp>
#include <common_ui/font.hpp>
#include <common_ui/text.hpp>
#include <common_ui/Theme.hpp>
#include <optional>

namespace aff::pk_high_low::views {

class system_check_view {
public:
    system_check_view() = default;
    ~system_check_view() = default;

    void tick(const SDL_Event* ev,
              aff::pk_high_low::controller::SettingsContext& ctx,
              aff::pk_high_low::controller::SettingsAPI& api);

private:
    bool is_set_up_ = false;
    aff::sdl_utils::widgets::Button set_launch_btn();

    Uint32 prev_t_ {0};
    sdl_config_bg bg_;
    aff::sdl_utils::widgets::ProgressBar progress_;
    aff::sdl_utils::widgets::Button launch_btn_;
    aff::sdl_utils::widgets::special::CircularEye eye_;

    // Title textures, font and layout for "StatDex.Check"
    std::optional<aff::sdl_utils::common::Font> title_font_;
    aff::sdl_utils::common::Texture title_left_tex_;
    aff::sdl_utils::common::Texture title_right_tex_;
    SDL_Rect title_left_rect_ { 0, 0, 0, 0 };
    SDL_Rect title_right_rect_ { 0, 0, 0, 0 };
    bool title_needs_update_ = true;
};

} // namespace aff::pk_high_low::views

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SYSTEMCHECK_VIEW_HPP

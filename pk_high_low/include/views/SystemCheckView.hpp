#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SYSTEMCHECK_VIEW_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SYSTEMCHECK_VIEW_HPP

#include <SDL2/SDL.h>
#include <controller/settings_api.hpp>
#include <common_ui/window.hpp>
#include <views/sdl_config_bg.hpp>

#include <widgets/ProgressBar.hpp>
#include <widgets/Button.hpp>

namespace aff::pk_high_low::views {

class system_check_view {
public:
    system_check_view() = default;
    ~system_check_view() = default;

    void tick(const SDL_Event* ev,
              aff::pk_high_low::controller::SettingsContext& ctx,
              aff::pk_high_low::controller::SettingsAPI& api);

private:
    sdl_config_bg bg_;
    aff::sdl_utils::widgets::ProgressBar progress_;
    aff::sdl_utils::widgets::Button launch_btn_;
};

} // namespace aff::pk_high_low::views

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_VIEWS_SYSTEMCHECK_VIEW_HPP

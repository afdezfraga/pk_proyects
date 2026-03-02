#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SETTINGS_CONTROLLER_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SETTINGS_CONTROLLER_HPP

#include <filesystem>
#include <common_ui/font.hpp>
#include <common_ui/text.hpp>
#include <common_ui/window.hpp>
#include <views/sdl_config_view.hpp>
#include <controller/settings_api.hpp>
#include <controller/controller_api.hpp>
#include <controller/game_mode.hpp>

namespace aff::pk_high_low::controller {

class SettingsController {
public:
    SettingsController(aff::sdl_utils::common::Window& window, const std::filesystem::path& assets_path);
    void tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api);
    void reset();
    game_settings& settings() { return settings_; }
    const game_settings& settings() const { return settings_; }

private:
    aff::sdl_utils::common::Window* window_;
    std::filesystem::path assets_path_;
    game_settings settings_;
    SettingsContext ctx_;
    SettingsAPI settings_api_ { .request = [](SettingsContext& ctx, SettingsAction a){ ctx.pending_action = a; } };
    aff::pk_high_low::views::sdl_config_view config_view_;
};

} // namespace

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SETTINGS_CONTROLLER_HPP

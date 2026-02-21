#ifndef AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SETTINGS_CONTROLLER_HPP
#define AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SETTINGS_CONTROLLER_HPP

#include <filesystem>
#include <common_ui/font.hpp>
#include <common_ui/text.hpp>
#include <common_ui/window.hpp>
#include <controller/controller_api.hpp>
#include <controller/game_mode.hpp>

namespace aff::pk_high_low::controller {

class SettingsController {
public:
    SettingsController(aff::sdl_utils::common::Window& window, const std::filesystem::path& assets_path);
    void tick(const SDL_Event* ev, AppContext& ctx, AppAPI& api);
    void reset();
    aff::pk_high_low::controller::game_settings& settings() { return settings_; }
    const aff::pk_high_low::controller::game_settings& settings() const { return settings_; }

private:
    aff::sdl_utils::common::Window* window_;
    std::filesystem::path assets_path_;
    aff::pk_high_low::controller::game_settings settings_;
    aff::sdl_utils::common::Font font_;
    int field_idx_ {0};
};

} // namespace

#endif // AFF_PK_PROJECTS_PK_HIGH_LOW_CONTROLLER_SETTINGS_CONTROLLER_HPP
